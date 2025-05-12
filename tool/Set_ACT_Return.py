import os
import re
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path

# 用户输入项
function_name = "npu_kernel_dev_ioctl"  # 目标函数名
new_value = "0"  # 设置值

global_id = None  # 初始化 global_id

os.environ["PATH"] = "/opt/vcast:" + os.environ.get("PATH", "")
os.environ["LM_LICENSE_FILE"] = "27000@10.126.29.229"

# 获取 global_id 从 HAL1/param.xml
def get_global_id(param_file_path, function_name):
    try:
        tree = ET.parse(param_file_path)
        root = tree.getroot()

        # 查找对应函数名的 <subprog> 元素
        subprog_elem = root.find(f".//subprog[@name='{function_name}']")
        if subprog_elem is not None:
            # 查找对应函数的 <param name="return"> 元素
            param_elem = subprog_elem.find(".//param[@name='return']")
            if param_elem is not None:
                global_id = param_elem.get("index")
                if global_id is not None:
                    return global_id
                else:
                    print(f"未找到 'index' 属性，返回 None")
            else:
                print(f"未找到 <param name='return'> 元素")
        else:
            print(f"未找到函数 {function_name} 对应的 <subprog> 元素")
    except Exception as e:
        print(f"解析 param.xml 失败: {e}")
    
    return None

# 获取参数文件路径（HAL1/param.xml）
def get_param_file_path(env_file):
    prefix = env_file.stem
    return Path(prefix) / "param.xml"

# 提取 <unit> 和 <subprogram> 的方法
def extract_unit_and_subprogram(xml_path):
    unit, subprogram = None, None
    try:
        with xml_path.open('r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            unit_match = re.search(r'<unit>(\d+)</unit>', content)
            subprogram_match = re.search(r'<subprogram>(\d+)</subprogram>', content)
            if unit_match and subprogram_match:
                unit = unit_match.group(1)
                subprogram = subprogram_match.group(1)
    except Exception as e:
        print(f"提取 <unit> 和 <subprogram> 失败: {e}")
    return unit, subprogram

# 获取 unit_name（.tu.c 文件的前缀）
def get_unit_name_from_tu_c(target_dir):
    tu_c_files = list(target_dir.glob('*.tu.c'))
    if tu_c_files:
        # 假设文件名格式是 unit_name.tu.c
        unit_name = tu_c_files[0].stem.split('.')[0]
        return unit_name
    else:
        print(f"{target_dir} 中未找到 .tu.c 文件")
        return None

# 获取 true_value 从 R-{dat_id}.XML 文件，从文件末尾开始查找
def get_true_value_from_xml(xml_path, harness_command_value):
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()

        # 查找所有 command_data 元素并从末尾开始查找匹配的 harness_command
        command_elems = root.findall(".//command_data")
        for command_elem in reversed(command_elems):  # 从文件末尾开始查找
            harness_command_elem = command_elem.find(".//harness_command")
            if harness_command_elem is not None and harness_command_elem.text == harness_command_value:
                actual_value_elem = command_elem.find(".//actual_value")
                if actual_value_elem is not None:
                    return actual_value_elem.text.strip()
                else:
                    print(f"未找到 <actual_value> 元素")
            else:
                print(f"未找到匹配的 <harness_command>: {harness_command_value}")
    except Exception as e:
        print(f"读取 XML 文件失败: {e}")
    
    return None

# 查找所有 .env 文件
for env_file in Path('.').glob('*.env'):
    # 获取 param.xml 文件路径
    param_file_path = get_param_file_path(env_file)

    # 获取 global_id
    global_id = get_global_id(param_file_path, function_name)
    if global_id is None:
        print("无法获取 global_id，退出")
        exit()

    print(f"从 {param_file_path} 获取 global_id: {global_id}")

    target_dir = Path(env_file.stem)  # .env 文件前缀作为目录名
    if not target_dir.is_dir():
        print(f"目录 {target_dir} 不存在，跳过")
        continue

    print(f"\n\033[34m进入目录: {target_dir}\033[0m")

    # 获取 unit_name（.tu.c 文件前缀）
    unit_name = get_unit_name_from_tu_c(target_dir)
    if unit_name is None:
        print(f"未能从 {target_dir} 获取 unit_name，跳过")
        continue

    index = 1
    while True:
        number_str = f"{index:03d}"
        full_function = f"{function_name}.{number_str}"
        pattern = f"<name>{full_function}</name>"

        found = False

        for xml_file in target_dir.rglob('*.XML'):
            try:
                with xml_file.open('r', encoding='utf-8', errors='ignore') as f:
                    for line_number, line in enumerate(f, 1):
                        if pattern in line:
                            found = True
                            print(f"\033[34m匹配 XML 文件: {xml_file} - 第 {line_number} 行 - {full_function}\033[0m")

                            # 提取 XML 文件名中的编号部分
                            xml_match = re.search(r'(\d+)\.XML$', xml_file.name)
                            if not xml_match:
                                print(f"无法从 XML 文件名中提取编号: {xml_file.name}")
                                break

                            dat_id = xml_match.group(1)
                            dat_filename = f"E-{dat_id}.DAT"
                            dat_path = target_dir / dat_filename
                            print(f"\033[34m对应 DAT 文件: {dat_path}\033[0m")

                            # 如果 .DAT 文件不存在，创建并写入拼接内容
                            if not dat_path.exists():
                                print(f"{dat_path} 不存在，创建并添加默认行")

                                # 提取 <unit> 和 <subprogram> 并拼接默认内容
                                unit, subprogram = extract_unit_and_subprogram(xml_file)
                                if unit and subprogram:
                                    new_line = f"0.{unit}.{subprogram}.{global_id}%{new_value}\n"
                                    with dat_path.open('w', encoding='utf-8') as df:
                                        df.write(new_line)
                                    print(f"已创建并写入新行到 {dat_path}: {new_line.strip()}")
                                else:
                                    print("无法提取 <unit> 或 <subprogram>，跳过写入")
                                break

                            # 如果 .DAT 文件已存在，且文件为空
                            if os.stat(dat_path).st_size == 0:
                                print(f"{dat_path} 是空文件，尝试添加默认行")
                                unit, subprogram = extract_unit_and_subprogram(xml_file)
                                if unit and subprogram:
                                    new_line = f"0.{unit}.{subprogram}.{global_id}%{new_value}\n"
                                    with dat_path.open('w', encoding='utf-8') as df:
                                        df.write(new_line)
                                    print(f"已写入新行到 {dat_path}: {new_line.strip()}")
                                else:
                                    print("无法提取 <unit> 或 <subprogram>，跳过写入")
                                break

                            # 正常修改最后一行，检查是否包含 %
                            with dat_path.open('r', encoding='utf-8') as df:
                                lines = df.readlines()

                            if not lines:
                                print(f"{dat_path} 是空文件，跳过")
                                break

                            last_line = lines[-1].strip()
                            if '%' not in last_line:
                                print(f"{dat_path} 最后一行不包含 '%'，添加拼接行")
                                unit, subprogram = extract_unit_and_subprogram(xml_file)
                                if unit and subprogram:
                                    new_line = f"0.{unit}.{subprogram}.{global_id}%{new_value}\n"
                                    with dat_path.open('a', encoding='utf-8') as df:  # 以追加模式打开文件
                                        df.write(new_line)
                                    print(f"已添加新行到 {dat_path}: {new_line.strip()}")
                                else:
                                    print("无法提取 <unit> 或 <subprogram>，跳过写入")
                            else:
                                print(f"{dat_path} 最后一行包含 '%', modify it")
                                prefix = last_line.split('%')[0]
                                new_line = f"{prefix}%{new_value}\n"
                                print(f"new_line:{new_line}")
                                lines[-1] = new_line
                                with dat_path.open('w', encoding='utf-8') as df:  # 以追加模式打开文件
                                        df.writelines(lines)
                            break

            except Exception as e:
                print(f"处理文件 {xml_file} 出错: {e}")

        if not found:
            print(f"\033[32m !!!Finish!!! <未找到匹配项: {full_function}，停止递增>\033[0m")
            break

        # 执行 clicast 命令
        clicast_command = f"clicast -e {target_dir.stem} -u {unit_name} -s {function_name} -t {full_function} EXecute run"
        print(f"\033[32m 执行命令: {clicast_command}\033[0m")
        try:
            subprocess.run(clicast_command, shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(f"------------------")
            
            # 执行失败时，从 R-{dat_id}.XML 获取 true_value
            harness_command_value = last_line.split('%')[0]  # 提取前缀，例如 0.9.27.2
            xml_file_path = target_dir / f"R-{dat_id}.XML"
            true_value = get_true_value_from_xml(xml_file_path, harness_command_value)
            if true_value:
                # 修改 .DAT 文件最后一行为 true_value
                print(f"修改 .DAT 文件最后一行为 true_value: {true_value}")
                lines[-1] = f"{prefix}%{true_value}\n"
                with dat_path.open('w', encoding='utf-8') as df:
                    df.writelines(lines)

                # 再次执行 clicast 命令
                clicast_command_retry = f"clicast -e {target_dir.stem} -u {unit_name} -s {function_name} -t {full_function} EXecute run"
                print(f"\033[33m重试执行命令: {clicast_command_retry}\033[0m")
                try:
                    subprocess.run(clicast_command_retry, shell=True, check=True)
                except subprocess.CalledProcessError as retry_error:
                    print(f"------------------")
        
        index += 1
