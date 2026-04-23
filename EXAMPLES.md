# FileCompressor - Usage Examples

This guide demonstrates various ways to use the FileCompressor tool.

## Quick Start

```bash
# Build the project
./build.sh              # Linux/macOS
build.bat              # Windows

# Generate test data
python3 generate_test_data.py -s 50 -t log

# Compress a file
./build/bin/compressor test_log_50mb.txt output.rle

# View results
ls -lh test_log_50mb.txt output.rle
```

## Example 1: Basic Compression

**Scenario:** Compress a small log file with default settings

```bash
./compressor input.log output.log.rle
```

**Output:**
```
>>> Starting File Compression Tool <<<
Input file: input.log (5242880 bytes)
Threads: 8 | Chunk size: 256 KB

[1/3] Splitting file into chunks...
      Chunks created: 20

[2/3] Compressing chunks with 8 threads...
      Progress: 20/20 chunks
      All chunks compressed!

[3/3] Merging compressed chunks...
      Output file created: output.log.rle

==================================================
  COMPRESSION STATISTICS
==================================================

Files:
  Input:  input.log
  Output: output.log.rle

Data Sizes:
  Original:         5242880 bytes
  Compressed:       2621440 bytes
  Ratio:            50.00%

Performance:
  Threads:              8
  Chunk Size:         256 KB
  Time:               0.043 seconds
  Throughput:      120.8 MB/s

==================================================
```

## Example 2: Large File with Custom Settings

**Scenario:** Compress a 1GB log file with 4 threads and 512KB chunks

```bash
./compressor -t 4 -c 512 large_app.log large_app.log.rle
```

**Recommendations:**
- Use 512KB chunks for faster I/O with large files
- Match thread count to CPU cores
- Monitor performance in the output statistics

## Example 3: Highly Repetitive Data

**Scenario:** Compress a file with lots of repeated patterns

```bash
# Generate test data
python3 generate_test_data.py -s 100 -t log -o testdata

# Compress with optimal settings for log files
./compressor -t 8 -c 256 testdata/test_log_100mb.txt output.rle
```

**Expected Results:**
- Log files typically compress to 40-60% of original size
- RLE works well with patterns like repeated spaces, punctuation
- High throughput due to minimal overhead

## Example 4: Random Data (Poor Compression)

**Scenario:** Attempt to compress random/binary data

```bash
# Generate random data
python3 generate_test_data.py -s 50 -t random -o testdata

# Compress
./compressor testdata/test_random_50mb.txt output.rle
```

**Expected Results:**
- Compression ratio near 0% (might even expand!)
- RLE is ineffective for random data
- Still measures throughput accurately

**Output Example:**
```
Data Sizes:
  Original:        52428800 bytes
  Compressed:      52823920 bytes
  Ratio:           -0.75%  (expansion due to RLE overhead)
```

## Example 5: Performance Benchmarking

**Scenario:** Compare performance with different thread counts

```bash
# Generate test file
python3 generate_test_data.py -s 100 -t log -o testdata

# Single thread
time ./compressor testdata/test_log_100mb.txt out1.rle

# Four threads
time ./compressor -t 4 testdata/test_log_100mb.txt out4.rle

# Eight threads
time ./compressor -t 8 testdata/test_log_100mb.txt out8.rle
```

**Typical Scaling:**
```
Threads: 1  → Time: 2.40s  → Throughput: 41.7 MB/s
Threads: 4  → Time: 0.65s  → Throughput: 153.8 MB/s  (3.7x faster)
Threads: 8  → Time: 0.35s  → Throughput: 285.7 MB/s  (6.8x faster)
```

## Example 6: Optimizing Chunk Size

**Scenario:** Find optimal chunk size for your hardware

```bash
# Small chunks (fast context switching)
./compressor -c 64 input.log output1.rle

# Medium chunks (balanced)
./compressor -c 256 input.log output2.rle

# Large chunks (better I/O)
./compressor -c 1024 input.log output3.rle
```

**Guidelines:**
- **64-256 KB**: Small files, SSD storage
- **256-512 KB**: General purpose (default)
- **1-4 MB**: Large files, HDD storage
- **4+ MB**: Massive files, high I/O bandwidth

## Example 7: Batch Processing Multiple Files

**Scenario:** Compress multiple log files

```bash
#!/bin/bash
# compress_batch.sh

for file in logs/*.log; do
    echo "Compressing $file..."
    ./compressor "$file" "${file}.rle"
done

# Show summary
du -sh logs/*.log logs/*.log.rle
```

**Run:**
```bash
chmod +x compress_batch.sh
./compress_batch.sh
```

## Example 8: Pipeline Optimization

**Scenario:** Monitor compression across different configurations

```bash
# Create monitoring script
cat > benchmark.sh << 'EOF'
#!/bin/bash
INPUT_FILE="$1"
SIZES=(64 128 256 512 1024)

echo "File: $INPUT_FILE"
echo "Threads,ChunkSize,Time,Throughput,Ratio"

for threads in 1 2 4 8; do
    for size in "${SIZES[@]}"; do
        ./compressor -t $threads -c $size "$INPUT_FILE" "out_${threads}_${size}.rle" 2>/dev/null | \
        grep -E "Time:|Throughput:|Ratio:" | paste -sd ',' -
    done
done
EOF

chmod +x benchmark.sh
./benchmark.sh testdata/test_log_100mb.txt
```

## Example 9: Memory-Efficient Compression

**Scenario:** Compress a very large file on memory-constrained system

```bash
# Use smaller chunks to reduce memory footprint
./compressor -t 2 -c 64 huge_file.log huge_file.rle
```

**Memory Estimation:**
```
Memory ≈ (Chunk Size × Number of Threads) + Overhead
Example: (64KB × 2) + ~10MB = ~10.1MB total
```

## Example 10: Parallel Compression of Different Files

**Scenario:** Compress multiple files in parallel at shell level

```bash
# Compress 4 files in parallel (4 processes × 2 threads each = 8 cores utilized)
./compressor -t 2 file1.log file1.rle &
./compressor -t 2 file2.log file2.rle &
./compressor -t 2 file3.log file3.rle &
./compressor -t 2 file4.log file4.rle &

wait
echo "All files compressed!"
```

## Performance Tips & Tricks

### 1. CPU Optimization
```bash
# Use CPU core count for threads
NUM_CORES=$(nproc)  # Linux/macOS
./compressor -t $NUM_CORES input.log output.rle
```

### 2. I/O Optimization
```bash
# For fast SSDs, use larger chunks
./compressor -c 1024 -t 8 input.log output.rle

# For slow HDDs, use more threads with smaller chunks
./compressor -c 128 -t 16 input.log output.rle
```

### 3. Real-time Monitoring
```bash
# Monitor compression in another terminal
watch -n 0.5 'du -sh output.rle'
```

### 4. Compression Verification
```bash
# Check output file size immediately after
ls -lh output.rle
wc -c output.rle  # Byte count
```

## Troubleshooting

### Issue: Low compression ratio
```bash
# Check file content
head -20 input.log

# Try with random data test
./compressor testdata/test_random_10mb.txt random.rle
# (Random data won't compress well - this is expected)
```

### Issue: Slow compression
```bash
# Check if using all threads
./compressor -t 8 input.log output.rle

# Monitor CPU usage
top  # (Linux/macOS)
taskmgr  # (Windows)

# Try different chunk sizes
./compressor -c 512 input.log output.rle
```

### Issue: Out of memory
```bash
# Reduce chunk size
./compressor -t 2 -c 32 huge_file.log output.rle
```

## Advanced Scenarios

### Scenario A: Network Distribution
```bash
# Compress on source machine
./compressor large_file.log large_file.rle

# Transfer compressed file (smaller)
scp large_file.rle user@remote:/destination/

# Decompress on destination (future feature)
# ./compressor -d large_file.rle large_file
```

### Scenario B: Archival Workflow
```bash
#!/bin/bash
# Archive script with compression

ARCHIVE_DIR="archive/$(date +%Y-%m-%d)"
mkdir -p "$ARCHIVE_DIR"

# Compress old logs
for log in logs/$(date -d "1 day ago" +%Y-%m-%d)*.log; do
    ./compressor "$log" "$ARCHIVE_DIR/$(basename $log).rle"
done

# Calculate saved space
du -sh logs/ "$ARCHIVE_DIR"
```

### Scenario C: Real-time Log Compression
```bash
#!/bin/bash
# Compress logs as they reach 100MB

while true; do
    LOG_FILE="app.log"
    SIZE=$(stat -f%z "$LOG_FILE" 2>/dev/null || stat -c%s "$LOG_FILE")
    
    if [ "$SIZE" -gt 104857600 ]; then
        TIMESTAMP=$(date +%s)
        ./compressor "$LOG_FILE" "${LOG_FILE}.${TIMESTAMP}.rle"
        > "$LOG_FILE"  # Clear log
    fi
    
    sleep 300  # Check every 5 minutes
done
```

---

**Happy compressing!** 📦
