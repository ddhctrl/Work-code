import sys
import re

def extract_undefined_refs(filename, output_filename):
    try:
        # 使用GBK编码打开文件
        with open(filename, 'r', encoding='gbk') as f:
            content = f.read()
            
        # 使用正则表达式匹配所有undefined reference后面的函数名
        pattern = r"undefined reference to ['`]([^'`]*)[`']"
        matches = re.findall(pattern, content)
        
        # 去重并排序
        unique_funcs = sorted(set(matches))
        
        # 将结果写入新文件
        with open(output_filename, 'w', encoding='utf-8') as f:
            for func in unique_funcs:
                f.write(func + '\n')
            
        print(f"Successfully extracted {len(unique_funcs)} functions to {output_filename}")
            
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python extract_undef.py <input_file>")
        sys.exit(1)
    
    # 创建输出文件名
    output_file = "undefined_functions.txt"
    extract_undefined_refs(sys.argv[1], output_file)

