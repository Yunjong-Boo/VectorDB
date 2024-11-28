# Graph Partitioning
This project focuses on **graph partitioning techniques** that leverage node connectivity to minimize the number of roundtrips for search alogirhtms. By considering the relationships between nodes and their shared parent nodes, this approach aims to minimize data access and improve query effciency in disk-based systems.

## Usage
1. Clone the Repository
```bash
    git clone https://github.com/Yunjong-Boo/VectorDB.git
    cd VectorDB
```
2. Build
 ```bash
     mkdir build
     cd build
     cmake ..
     make -j
 ```
3. Dataset Preparation
 - SIFT1M (bigann링크)
 - 주의사항: data type에 따라 uint8_t/float 변경 필요
4. Run Graph Conversion
 ```bash
 # In build directory
 ./WeightCalculation <path_to_data_file> <path_to_output_file>
 ```
5. Graph Partitioning
- PuLP(링크)


