#!/usr/bin/env python3

import re
import io

head = 'Test Case Description:'
body = '     Cover statement branch, verify the function is executed correctly.No branches in subprogram.'
tail = '''
Methods for software unit testing:
Requirements-based test,Interface test

Methods for deriving test cases for software unit testing:
Analysis of requirements,Generation and analysis of equivalence classes,Analysis of boundary values'''


def handle_test_note(origin: str) -> str:
    if 'This is an automatically generated test case.' in origin:
        result = origin.replace('This is an automatically generated test case.', head)
        result = re.sub(r'\s*Test Case Generation Notes:(.*)', '\n' + tail, result, flags=re.DOTALL)
    else:
        result = '\n'.join([head, body, tail])
    return result


def handle_test_case(test_case: str) -> str:
    test_name = re.findall(r'TEST\.NAME:(.*)', test_case)[0]
    test_num = re.findall(r'\d{3}', test_name)[0]
    test_subprogram = re.findall(r'TEST\.SUBPROGRAM:(.*)', test_case)[0]
    result = re.sub(r'(-- Test Case: )(.*)', rf'\1{test_subprogram}.{test_num}', test_case)
    result = re.sub(r'(TEST\.NAME:)(.*)', rf'\1{test_subprogram}.{test_num}', result)
    test_notes = re.findall(r'TEST\.NOTES:\n(.*?)TEST\.END_NOTES:', result, flags=re.DOTALL)[0]
    new_test_notes = handle_test_note(test_notes)
    result = re.sub(r'(TEST\.NOTES:\n)(.*?)(TEST\.END_NOTES:)', rf'\1{new_test_notes}\n\3', result, flags=re.DOTALL)
    return result


def get_head(s: str):
    m = re.match(r'(.*?)-- Test Case:', s, flags=re.DOTALL)
    return m.groups()[0]


if __name__ == '__main__':
    with open(r'in.tst', 'r', encoding='utf-8') as f:
        content = f.read()
    
    tst_head = get_head(content)
    
    stringIO = io.StringIO()
    stringIO.write(tst_head)
    
    test_cases = re.findall(r'-- Test Case:.*?TEST\.END\n', content, flags=re.DOTALL)
    
    for test_case in test_cases:
        new_test_case = handle_test_case(test_case)
        stringIO.write(new_test_case)
        stringIO.write('\n')
    
    result = stringIO.getvalue()
    with open(r'out.tst', 'w', encoding='utf-8') as f2:
        f2.write(result)
