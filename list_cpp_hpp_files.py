import os

def list_cpp_hpp_files(directory):
    cpp_hpp_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.hpp') or file == 'Makefile':
                file_path = os.path.join(root, file)
                cpp_hpp_files.append(file_path)
    return cpp_hpp_files

def read_file_content(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()
    return content

def write_to_output_file(files, output_file_path):
    with open(output_file_path, 'w', encoding='utf-8') as output_file:
        output_file.write("我现在有这些文件：\n")
        for file in files:
            content = read_file_content(file)
            output_file.write(f"文件名：{file}\n")
            output_file.write("内容：\n")
            output_file.write(content)
            output_file.write("\n\n")  # 空行分隔不同文件的内容

def main():
    directory = '.'  # 根目录
    output_file_path = 'output.txt'  # 输出文件
    files = list_cpp_hpp_files(directory)
    
    if files:
        write_to_output_file(files, output_file_path)
        print(f"所有文件内容已保存到：{output_file_path}")
    else:
        print("没有找到任何 .cpp 或 .hpp 文件或Makefile文件。")

if __name__ == "__main__":
    main()

# I型（广域歼灭型）：消除的条的上下两条会一起消除
# O型（强力溅射型）：随机消除3行
# T型（点石成金型）：使本次消除得分*5
# S型（迎头痛击型）：最上面的一行也会消失
# Z型（釜底抽薪型）：最底下的一条也会消失
# J型（女娲补天型）：下三个生成的块将会是一种特殊的块，它只由一个小的块构成，这个小块如果消除了某个行，则会增加一次小块的生成
# L型（异核熔解型）：下一个生成的块将会是另一种小块，如果这个小块消除了某个行，则会把底下的5行清除