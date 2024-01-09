import re

def extract_content_between_patterns(file_path, pattern1, pattern2):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    capturing = False
    content_between_patterns = []
    for line in lines:
        if re.search(pattern1, line) and not capturing:
            capturing = True
            continue
        elif re.search(pattern2, line) and capturing:
            capturing = False
            continue
        if capturing:
            content_between_patterns.append(line.strip())

    return content_between_patterns

# 示例：假设 pattern1 和 pattern2 是您想要查找的两个正则表达式模式
pattern1 = r"YOUR_FIRST_PATTERN_HERE"
pattern2 = r"YOUR_SECOND_PATTERN_HERE"
file_path = "YOUR_FILE_PATH_HERE"

# 调用函数（需要替换上面的占位符）
# extracted_content = extract_content_between_patterns(file_path, pattern1, pattern2)


def extract_content_between_multiple_patterns(file_path, pattern_pairs):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    results = []
    current_pattern_end = None
    current_content = []

    for line in lines:
        if current_pattern_end:
            # 检查是否到达了当前模式对的结束模式
            if re.search(current_pattern_end, line):
                results.append(current_content)
                current_content = []
                current_pattern_end = None
            else:
                current_content.append(line.strip())
        else:
            for start_pattern, end_pattern in pattern_pairs:
                if re.search(start_pattern, line):
                    current_pattern_end = end_pattern
                    break

    return results

# 示例：假设有多个正则表达式模式对
pattern_pairs = [
    (r"START_PATTERN_1", r"END_PATTERN_1"),
    (r"START_PATTERN_2", r"END_PATTERN_2"),
    # 可以添加更多模式对
]
file_path = "YOUR_FILE_PATH_HERE"

# 调用函数（需要替换上面的占位符）
# extracted_contents = extract_content_between_multiple_patterns(file_path, pattern_pairs)

import os

def find_and_process_files(directory, filename_pattern, pattern_pairs):
    matched_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if re.search(filename_pattern, file):
                matched_files.append(os.path.join(root, file))

    all_extracted_contents = {}
    for file_path in matched_files:
        extracted_contents = extract_content_between_multiple_patterns(file_path, pattern_pairs)
        all_extracted_contents[file_path] = extracted_contents

    return all_extracted_contents

# 示例：目录路径和文件名模式
directory = "YOUR_DIRECTORY_PATH_HERE"
filename_pattern = r"YOUR_FILENAME_PATTERN_HERE"  # 例如，r"*.txt" 匹配所有 .txt 文件

# 调用函数（需要替换上面的占位符）
# results = find_and_process_files(directory, filename_pattern, pattern_pairs)

# 结果 'results' 将是一个字典，键是匹配的文件路径，值是从这些文件中提取的内容。
