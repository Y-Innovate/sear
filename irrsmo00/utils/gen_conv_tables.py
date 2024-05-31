import ebcdic

def gen_table(code_page = 'cp1047', encrypt = True, row_size = 16):
    list = []
    if encrypt:
        print('static const unsigned char AsciiToEbcdic[256] = {')
    else:
        print('static const unsigned char EbcdicToAscii[256] = {')
    for i in range(0, 256, row_size):
        new_str = "   "
        for j in range(row_size):
            new_str = new_str + ' '
            if not encrypt:
                new_str = new_str + ("%s," % hex(ord((i+j).to_bytes(1).decode(code_page))))
                list.append(hex(ord((i+j).to_bytes(1).decode(code_page))))
            else:
                new_str = new_str + ("%s," % hex(ord(chr(i+j).encode(code_page))))
                list.append(hex(ord(chr(i+j).encode(code_page))))
        print(new_str)
    print("};")
    return list

a2e = gen_table(encrypt=False)
e2a = gen_table()

for char in "abcdefghijklmnopqrstuvwxyzACBDEFGHIJKLMNOPQRSTUVWXYZ":
    print(char)
    print(a2e[ord(char)])
    print(char.encode('cp1047'))