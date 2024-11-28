#include <cxxopts.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <functional>
#include <set>
#include "diskindex_data.hpp"
#include "config.hpp"


void edgeweightcalculation(const std::string& filename_disk_index, const std::string& filename_output){
  auto disk_index_data = DiskIndexData();
  disk_index_data.Load(filename_disk_index);
  
  disk_index_data.edgeweightcalculation();


  size_t npts = disk_index_data.getSize();

  std::ofstream file(filename_output);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename_output << std::endl;
    return;
  }

  file << npts << " "<< disk_index_data.get_sharing_edges_count()/2 << " 001" << std::endl;
  
  for(int vi = 0; vi < disk_index_data.getSize(); vi++) {
    auto sharing_edge = disk_index_data.getSharingEdge(vi);
    for (auto edge :sharing_edge) {
      if(edge.first == vi ) continue;
      file << edge.first+1 << " " << edge.second << " ";  //id +1
    }
    file << std::endl;
  }
}

int main(int argc, char* argv[]) {
  std::string filename_disk_index;
  std::string filename_output;

  try {
    cxxopts::Options options(argv[0], " - example command line options");

    // Define options
    options.add_options()
        ("disk_index", "disk_index filename", cxxopts::value<std::string>()) 
        ("output", "output filename", cxxopts::value<std::string>()) 
        ("h,help", "Print help");
    
    options.parse_positional({"disk_index", "output"});

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    }

    if (result.count("disk_index")) {
      filename_disk_index = result["disk_index"].as<std::string>();
      std::cout << "disk_index: " << filename_disk_index << std::endl;
    }
    else {
      throw cxxopts::OptionException("Specify disk_index file name.");
    }

    if (result.count("output")) {
      filename_output = result["output"].as<std::string>();
      std::cout << "output: " << filename_output << std::endl;
    }
    else {
      throw cxxopts::OptionException("Specify output file name.");
    }
  } catch (const cxxopts::OptionException& e) {
    std::cerr << "Error parsing options: " << e.what() << std::endl;
    return 1;
  }

  edgeweightcalculation(filename_disk_index,filename_output);

  return 0;
}
