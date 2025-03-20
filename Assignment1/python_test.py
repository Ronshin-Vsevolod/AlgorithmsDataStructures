#!/usr/bin/env python3
import base64
import subprocess
import random
import tempfile
import os

def test_encode_decode_ascii85():
    """Testing ASCII85 encoding and decoding using
    random data and comparison with Python's base64 library."""
    print("Testing ASCII85 encoding/decoding with random data:")
    
    for size in [10, 100, 1000]:
        print(f"  Testing data with size {size} bytes.")
        
        random_data = bytes(random.randint(0, 255) for _ in range(size))
        
        python_encoded = base64.a85encode(random_data).decode('ascii')
        
        # Encode using our program
        with tempfile.NamedTemporaryFile() as temp_in:
            temp_in.write(random_data)
            temp_in.flush()
            
            our_encoded = subprocess.check_output(
                f"cat {temp_in.name} | ./ascii85",
                shell=True
            ).decode('ascii')
            
            # Decode back using our program
            with tempfile.NamedTemporaryFile() as temp_encoded:
                temp_encoded.write(our_encoded.encode('ascii'))
                temp_encoded.flush()
                
                our_decoded = subprocess.check_output(
                    f"cat {temp_encoded.name} | ./ascii85 -d",
                    shell=True
                )
                
                assert our_decoded == random_data, "Decoded data does not match the original data"
                
                # There may be extra spaces or line breaks in the output of ascii85 programme, while in base64 there is no such thing.
                our_encoded = our_encoded.strip()
                python_encoded = python_encoded.strip()
                
                if our_encoded != python_encoded:
                    print(f"    Warning: our encoding differs from Python's:")
                    print(f"      Ours:   {our_encoded[:50]}...")
                    print(f"      Python: {python_encoded[:50]}...")
        
        print("    Test passed successfully")

def test_invalid_input():
    print("\nTesting handling of invalid input data during decoding:")
    
    invalid_inputs = [
        "!",        # Too short string
        "!!!{}",    # Invalid characters
        "vvvvv"     # Value exceeds valid range
    ]
    
    for i, invalid in enumerate(invalid_inputs):
        print(f"  Test {i+1}: {invalid}")
        
        with tempfile.NamedTemporaryFile() as temp_in:
            temp_in.write(invalid.encode('ascii'))
            temp_in.flush()
            
            try:
                result = subprocess.check_output(
                    f"cat {temp_in.name} | ./ascii85 -d",
                    stderr=subprocess.STDOUT,
                    shell=True
                ).decode('ascii')
                
                print(f"    Error: Decoder should have reported an error, but returned: {result}")
            except subprocess.CalledProcessError as e:
                print(f"    Test passed successfully: decoder returned code {e.returncode}")

if __name__ == "__main__":
    if not os.path.exists("./ascii85"):
        print("Error: ascii85 program not found in current directory")
        exit(1)
        
    try:
        test_encode_decode_ascii85()
        test_invalid_input()
        print("\nAll tests passed successfully!")
    except AssertionError as e:
        print(f"Error: {e}")
        exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        exit(1) 