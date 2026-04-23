#!/usr/bin/env python3
"""
Test Data Generator for FileCompressor

Generates various test files with different characteristics:
- Repetitive logs (RLE-friendly)
- Random text (RLE-unfriendly)
- Mixed patterns
"""

import sys
import argparse
from pathlib import Path

def generate_repetitive_log(filename, size_mb):
    """Generate a log file with high repetition (good for RLE)"""
    print(f"Generating repetitive log file: {filename} ({size_mb}MB)...")
    
    log_templates = [
        "[INFO] Processing request from user {}\n",
        "[DEBUG] Database query executed in {}ms\n",
        "[WARNING] Cache miss for key: {}\n",
        "[ERROR] Connection timeout after {}s\n",
        "[INFO] User {} logged in successfully\n",
    ]
    
    target_size = size_mb * 1024 * 1024
    current_size = 0
    user_id = 1
    
    with open(filename, 'w') as f:
        while current_size < target_size:
            for template in log_templates:
                if "[INFO]" in template:
                    log_line = template.format(user_id % 1000)
                elif "[DEBUG]" in template:
                    log_line = template.format((user_id * 73) % 500)
                elif "[WARNING]" in template:
                    log_line = template.format(f"user_{user_id % 100}")
                elif "[ERROR]" in template:
                    log_line = template.format((user_id * 17) % 60)
                else:
                    log_line = template.format(user_id % 500)
                
                f.write(log_line)
                current_size += len(log_line)
                user_id += 1
                
                if current_size >= target_size:
                    break
    
    print(f"✓ Created {filename} ({current_size} bytes)")

def generate_random_text(filename, size_mb):
    """Generate random text (bad for RLE)"""
    print(f"Generating random text file: {filename} ({size_mb}MB)...")
    
    import random
    import string
    
    target_size = size_mb * 1024 * 1024
    current_size = 0
    
    with open(filename, 'w') as f:
        while current_size < target_size:
            # Random line of alphanumeric characters
            line_length = random.randint(50, 150)
            line = ''.join(random.choices(string.ascii_letters + string.digits + ' ', 
                                         k=line_length)) + '\n'
            f.write(line)
            current_size += len(line)
    
    print(f"✓ Created {filename} ({current_size} bytes)")

def generate_mixed_data(filename, size_mb):
    """Generate mixed pattern data"""
    print(f"Generating mixed pattern file: {filename} ({size_mb}MB)...")
    
    target_size = size_mb * 1024 * 1024
    current_size = 0
    
    patterns = [
        "AAABBBCCCDDDEEEFFF\n" * 100,  # Highly repetitive
        "The quick brown fox jumps over the lazy dog\n" * 50,  # Regular text
        "x" * 100 + "\n",  # Single character run
        "abcdefghijklmnopqrstuvwxyz\n" * 20,  # Low repetition
    ]
    
    with open(filename, 'w') as f:
        pattern_idx = 0
        while current_size < target_size:
            pattern = patterns[pattern_idx % len(patterns)]
            f.write(pattern)
            current_size += len(pattern)
            pattern_idx += 1
    
    print(f"✓ Created {filename} ({current_size} bytes)")

def main():
    parser = argparse.ArgumentParser(
        description="Generate test files for FileCompressor benchmarking"
    )
    parser.add_argument('-s', '--size', type=int, default=10,
                       help='File size in MB (default: 10)')
    parser.add_argument('-t', '--type', choices=['log', 'random', 'mixed', 'all'],
                       default='log', help='Type of test file (default: log)')
    parser.add_argument('-o', '--output', default='.',
                       help='Output directory (default: current directory)')
    
    args = parser.parse_args()
    
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print("=" * 60)
    print("  Test Data Generator for FileCompressor")
    print("=" * 60)
    print()
    
    if args.type == 'log' or args.type == 'all':
        log_file = output_dir / f"test_log_{args.size}mb.txt"
        generate_repetitive_log(str(log_file), args.size)
    
    if args.type == 'random' or args.type == 'all':
        random_file = output_dir / f"test_random_{args.size}mb.txt"
        generate_random_text(str(random_file), args.size)
    
    if args.type == 'mixed' or args.type == 'all':
        mixed_file = output_dir / f"test_mixed_{args.size}mb.txt"
        generate_mixed_data(str(mixed_file), args.size)
    
    print()
    print("=" * 60)
    print("  Test files ready for compression!")
    print("=" * 60)

if __name__ == '__main__':
    main()
