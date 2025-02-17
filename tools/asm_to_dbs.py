def binary_to_nasm(input_file, output_file):
    try:
        with open(input_file, 'rb') as bin_file:
            binary_data = bin_file.read()

        with open(output_file, 'w') as asm_file:
            # Write each byte as a NASM `db` statement
            for i in range(0, len(binary_data), 16):  # 16 bytes per line for readability
                chunk = binary_data[i:i+16]
                db_line = ", ".join([f"0x{byte:02x}" for byte in chunk])
                asm_file.write(f"db {db_line}\n")

        print(f"Successfully wrote NASM commands to {output_file}")
    except FileNotFoundError:
        print(f"Error: File {input_file} not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
if __name__ == "__main__":
    input_path = input("Enter the path to the binary file: ").strip()
    output_path = input("Enter the path to the output .asm file: ").strip()
    binary_to_nasm(input_path, output_path)

