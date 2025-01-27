import os
import glob

def get_environment_name():
    # 在当前目录下查找以 .env 结尾的文件
    env_files = glob.glob("*.env")
    
    if env_files:
        # 获取第一个 .env 文件的文件名作为环境名称（去除扩展名）
        return os.path.splitext(env_files[0])[0]
    else:
        print("环境错误, 找不到.env, 请检查")
        return None

def insert_functions_after_library_stubs(functions_file, env_name, prefix="ENVIRO.ADDITIONAL_STUB:"):
    try:
        if env_name is None:
            return

        # 构造目标文件路径
        target_file = f"{env_name}/ENVIRO.AUX"
        
        # 读取包含函数名的文件
        with open(functions_file, 'r') as f:
            functions = f.readlines()
        
        # 去掉每行的换行符
        functions = [func.strip() for func in functions]
        
        # 读取ENVIRO.AUX文件内容
        with open(target_file, 'r') as f:
            lines = f.readlines()
        
        # 找到插入位置，即ENVIRO.LIBRARY_STUBS:后面的一行
        insert_position = None
        for i, line in enumerate(lines):
            if "ENVIRO.LIBRARY_STUBS:" in line:
                insert_position = i + 1  # 在该行后面插入
                break
        
        if insert_position is not None:
            # 插入函数名
            for func in functions:
                lines.insert(insert_position, f"{prefix}{func}\n")
                insert_position += 1  # 更新插入位置
        
            # 将修改后的内容写回到ENVIRO.AUX文件
            with open(target_file, 'w') as f:
                f.writelines(lines)
            
            print(f"Successfully insert stub functions to {target_file}")
        else:
            print(f"未找到插入位置（ENVIRO.LIBRARY_STUBS:），请检查文件格式")
    
    except Exception as e:
        print(f"发生错误: {e}")

if __name__ == "__main__":
    functions_file = "undefined_functions.txt"  # 存储函数名的文件路径

    # 获取环境名称
    env_name = get_environment_name()

    # 根据环境名称插入函数
    insert_functions_after_library_stubs(functions_file, env_name)

