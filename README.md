# Graph Partitioning
This project focuses on **graph partitioning techniques** that leverage node connectivity to minimize the number of roundtrips for search alogirhtms. By considering the relationships between nodes and their shared parent nodes, this approach aims to minimize data access and improve query effciency in disk-based systems.

## Usage
### 1. Clone the Repository
 ```bash
    git clone https://github.com/Yunjong-Boo/VectorDB.git
    cd VectorDB
 ```
### 2. Build
 ```bash
     mkdir build
     cd build
     cmake ..
     make -j
 ```
### 3. Dataset Preparation
   This project uses the BIGANN Dataset and the DiskANN indexing framework.
   Follow the steps below to prepare the dataset and build the index:
   #### Step 1: Download the BIGANN Dataset
    1. Access the BIGANN Dataset from the following link: [BIGANN Dataset](http://corpus-texmex.irisa.fr/)
    2. Download the desired dataset files.
   #### Step 2: Build the DiskANN Index
    [DiskANN](https://github.com/microsoft/DiskANN)
    1. Clone the DiskANN repository from GitHub:
    ```bash
    git clone https://github.com/microsoft/DiskANN.git
    cd DiskANN
    ```
    2. Detailed build instructions are provided in the DiskANN GitHub repository under the following link: [DiskANN Index Build](https://github.com/microsoft/DiskANN/blob/main/workflows/SSD_index.md)


### 4. Run Graph Conversion
 ```bash
 # In build directory
 ./WeightCalculation <path_to_data_file> <path_to_output_file>
 ```
### 5. Graph Partitioning
    [PuLP](https://github.com/HPCGraphAnalysis/PuLP)
    1. Clone the PuLP repository from GitHub:
     ```bash
     git clone https://github.com/HPCGraphAnalysis/PuLP
     cd PuLP/pulp/0.2
     ```
    2. Build and run pulp
     ```bash
      make
      ./pulp <graphfile built by DiskANN> <num parts>
     ```

