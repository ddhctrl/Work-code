import os
import re
import xml.etree.ElementTree as ET
from pathlib import Path

# 用户输入项
function_name = "npu_kernel_dev_ioctl"  # 目标函数名
new_value = "1"  # 设置值

#
global_id = None  # 初始化 global_id

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

    print(f"\n进入目录: {target_dir}")

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
                            print(f"匹配 XML 文件: {xml_file} - 第 {line_number} 行 - {full_function}")

                            # 提取 XML 文件名中的编号部分
                            xml_match = re.search(r'(\d+)\.XML$', xml_file.name)
                            if not xml_match:
                                print(f"无法从 XML 文件名中提取编号: {xml_file.name}")
                                break

                            dat_id = xml_match.group(1)
                            dat_filename = f"E-{dat_id}.DAT"
                            dat_path = target_dir / dat_filename
                            print(f"对应 DAT 文件: {dat_path}")

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
                                lines[-1] = new_line
                                with dat_path.open('w', encoding='utf-8') as df:  # 以追加模式打开文件
                                        df.writelines(lines)
                            break

            except Exception as e:
                print(f"处理文件 {xml_file} 出错: {e}")

        if not found:
            print(f"未找到匹配项: {full_function}，停止递增")
            break

        index += 1

