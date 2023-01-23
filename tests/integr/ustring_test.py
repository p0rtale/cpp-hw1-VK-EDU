import argparse
import subprocess

exe_path = "ustring"


class bcolors:
    OK = '\033[92m'
    FAILED = '\033[91m'
    ENDC = '\033[0m'

class message:
    OK = f"{bcolors.OK}OK{bcolors.ENDC}"
    FAILED = f"{bcolors.FAILED}FAILED{bcolors.ENDC}"


def test(name, input, expected):
    process=subprocess.Popen(
        [exe_path],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding='utf8'
    )
    output, _ = process.communicate(input=input)

    if output == expected: 
        print(f"{name}: {message.OK}")
    else:
        print(f"{name}: {message.FAILED} [{repr(expected)} != {repr(output)}]")

def run_tests():
    test("TestOneChar", "pushc Ю show exit\n", "Ю\n")
    test(
        "TestMultipleChars",
        "pushc Ц show pushc 囎 show pushc パ show exit\n",
        "Ц\nЦ囎\nЦ囎パ\n"
    )

    # https://onlineutf8tools.com/convert-utf8-to-hexadecimal

    test("TestCodePointOne", f"pushp 84 show exit\n", "T\n")
    test("TestCodePointTwo", f"pushp 1048 show exit\n", "И\n")
    test("TestCodePointThree", f"pushp 12473 show exit\n", "ス\n")

    test("TestOneByte", f"pushb 1 {0x47} show exit\n", "G\n")
    test("TestTwoBytes", f"pushb 2 {0xd0} {0x96} show exit\n", "Ж\n")
    test("TestThreeBytes", f"pushb 3 {0xe3} {0x83} {0x9f} show exit\n", "ミ\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", dest="path", required=True, type=str)
    args = parser.parse_args()

    exe_path = args.path
    run_tests()
