import struct
import sys
import os

def list_wad_files(wad_file_path):
    try:
        with open(wad_file_path, 'rb') as wad_file:
            header = wad_file.read(12)
            identifier, num_lumps, directory_offset = struct.unpack('4sII', header)

            if identifier not in {b'IWAD', b'PWAD'}:
                print("Invalid WAD file identifier.")
                return

            print(f"WAD Type: {identifier.decode('ascii')}")
            print(f"Number of lumps: {num_lumps}")
            print(f"Directory offset: {directory_offset}")

            wad_file.seek(directory_offset)

            print("\nLumps:")
            for _ in range(num_lumps):
                entry_data = wad_file.read(16)
                lump_offset, lump_size, lump_name = struct.unpack('II8s', entry_data)
                lump_name = lump_name.decode('ascii').rstrip('\x00')
                print(f"Name: {lump_name}, Offset: {lump_offset}, Size: {lump_size}")

    except FileNotFoundError:
        print(f"Error: File '{wad_file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def show_wad_file(wad_file_path, lump_name):
    try:
        with open(wad_file_path, 'rb') as wad_file:
            header = wad_file.read(12)
            identifier, num_lumps, directory_offset = struct.unpack('4sII', header)

            if identifier not in {b'IWAD', b'PWAD'}:
                print("Invalid WAD file identifier.")
                return

            wad_file.seek(directory_offset)

            for _ in range(num_lumps):
                entry_data = wad_file.read(16)
                lump_offset, lump_size, lump_name_bytes = struct.unpack('II8s', entry_data)
                lump_name_read = lump_name_bytes.decode('ascii').rstrip('\x00')

                if lump_name_read == lump_name:
                    print(f"Name: {lump_name_read}, Offset: {lump_offset}, Size: {lump_size}")
                    wad_file.seek(lump_offset)
                    lump_data = wad_file.read(lump_size)
                    print(f"Data: {lump_data}")
                    save_to_file(f"{lump_name}.lmp", lump_data)
                    break

    except FileNotFoundError:
        print(f"Error: File '{wad_file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def save_to_file(file_path, data):
    with open(file_path, 'wb') as file:
        file.write(data)

def pack_wad(output_path, files):
    try:
        lump_entries = []
        lump_data = b''

        current_offset = 12  # Initial offset after the header

        for file_path in files:
            with open(file_path, 'rb') as f:
                data = f.read()
                lump_name = os.path.basename(file_path)[:8].encode('ascii').ljust(8, b'\x00')  # Ensure 8-byte name as bytes
                lump_entries.append((current_offset, len(data), lump_name))
                lump_data += data
                current_offset += len(data)

        num_lumps = len(lump_entries)
        directory_offset = current_offset

        with open(output_path, 'wb') as wad_file:
            # Write WAD header
            wad_file.write(struct.pack('4sII', b'IWAD', num_lumps, directory_offset))

            # Write lump data
            wad_file.write(lump_data)

            # Write lump directory
            for offset, size, name in lump_entries:
                wad_file.write(struct.pack('II8s', offset, size, name))

        print(f"WAD file '{output_path}' created successfully with {num_lumps} lumps.")

    except Exception as e:
        print(f"An error occurred while packing the WAD: {e}")

if __name__ == "__main__":
    # python wad.py list <wad_file_path>
    # python wad.py show <wad_file_path> <lump_name>
    # python wad.py pack <output_path> <file1> <file2> ... <fileN>
    if len(sys.argv) < 3:
        print("Usage: python wad.py <list|show|pack> <args>")
        sys.exit(1)

    command = sys.argv[1]

    if command == 'list':
        list_wad_files(sys.argv[2])
    elif command == 'show':
        show_wad_file(sys.argv[2], sys.argv[3])
    elif command == 'pack':
        pack_wad(sys.argv[2], sys.argv[3:])
    else:
        print("Invalid command. Use 'list', 'show', or 'pack'.")
        sys.exit(1)
