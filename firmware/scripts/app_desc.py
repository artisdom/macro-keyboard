import struct
import argparse



def hexify(s, uppercase=True):
    format_str = "%02X" if uppercase else "%02x"
    return "".join(format_str % c for c in s)


def decode_app_desc(filename):
    file = open(filename,'rb')

    app_desc = file.read()

    APP_DESC_STRUCT_FMT = "<II" + "8s" + "32s32s16s16s32s32s" + "80s"
    (
        magic_word,
        secure_version,
        reserv1,
        version,
        project_name,
        time,
        date,
        idf_ver,
        app_elf_sha256,
        reserv2,
    ) = struct.unpack(APP_DESC_STRUCT_FMT, app_desc)

    if magic_word == 0xABCD5432:
        title = "Application information"
        print(title)
        print("=" * len(title))
        print(f'Project name: {project_name.decode("utf-8")}')
        print(f'App version: {version.decode("utf-8")}')
        print(f'Compile time: {date.decode("utf-8")} {time.decode("utf-8")}')
        print(f"ELF file SHA256: {hexify(app_elf_sha256, uppercase=False)}")
        print(f'ESP-IDF: {idf_ver.decode("utf-8")}')
        print(f"Secure version: {secure_version}")
    else:
        print("Error: MAGIC_WORD not found")

    file.close()




if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Decode the esp_app_desc_t from a binary file")
    parser.add_argument('filename', default="app_desc.bin")

    args = parser.parse_args()

    decode_app_desc(args.filename)
