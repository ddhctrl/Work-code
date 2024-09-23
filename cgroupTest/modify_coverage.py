import sys

def modify_coverage(info_file, function_name, new_count):
    modified_lines = []
    fn_found = False
    line_number = None

    with open(info_file, 'r') as file:
        lines = file.readlines()
    
    for line in lines:
        if line.startswith('FN:'):
            # 检查是否是目标函数
            parts = line.split(',')
            if len(parts) == 2 and parts[1].strip() == function_name:
                line_number = parts[0].split(':')[1].strip()
                fn_found = True
            modified_lines.append(line)
        elif line.startswith('FNDA:') and fn_found:
            # 修改 FNDA 行
            parts = line.split(',')
            if len(parts) == 2 and parts[1].strip() == function_name:
                modified_line = f"FNDA:{new_count},{function_name}\n"
                modified_lines.append(modified_line)
                fn_found = False  # 只修改第一次出现的
            else:
                modified_lines.append(line)
        elif line.startswith('DA:') and line_number is not None:
            # 修改 DA 行
            parts = line.split(',')
            if len(parts) == 2 and parts[0].split(':')[1].strip() == line_number:
                modified_line = f"DA:{line_number},{new_count}\n"
                modified_lines.append(modified_line)
                line_number = None  # 只修改对应的第一次出现的
            else:
                modified_lines.append(line)
        else:
            modified_lines.append(line)

    with open(info_file, 'w') as file:
        file.writelines(modified_lines)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("用法: python3 modify_coverage.py <info文件> <函数名> <新的执行次数>")
        sys.exit(1)

    info_file = sys.argv[1]
    function_name = sys.argv[2]
    new_count = sys.argv[3]

    modify_coverage(info_file, function_name, new_count)
    print(f"{function_name} 的执行次数已修改为 {new_count}，并更新了对应行数的覆盖次数。")
