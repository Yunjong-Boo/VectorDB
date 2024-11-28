#ifndef __DISKINDEX_DATA_HPP__
#define __DISKINDEX_DATA_HPP__


#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include "config.hpp"
#include <algorithm>
#include <iterator>
class DiskIndexData {
public:
  DiskIndexData() {};

  bool Load(const std::string& filename); 
  uint32_t getDim(){return dim;}
  uint32_t getSize(){return npts;}
  uint32_t getMaxDegree(){return max_degree;}
  uint32_t getStartIdx(){return start_idx;}

  // Function to get a reference to the i-th vector
  std::vector<float>& getVector(size_t i) {
    // Add boundary check to prevent out-of-range access
    if (i >= data.size()) {
      throw std::out_of_range("Index out of range");
    }

    loaded_pages.insert(get_page_idx(i));

    return data[i];
  }
  
    // Function to get a reference to the i-th vector
  std::vector<uint32_t>& getNeighbors(size_t i) {
    // Add boundary check to prevent out-of-range access
    if (i >= neighbors.size()) {
      throw std::out_of_range("Index out of range");
    }

    loaded_pages.insert(get_page_idx(i));

    return neighbors[i];
  }
  
  void reset_loaded_pages() {loaded_pages.clear();}
  size_t get_loaded_pages() {return loaded_pages.size();}

  std::vector<std::pair<uint32_t,uint32_t>>& getSharingEdge(size_t i) {
    // Add boundary check to prevent out-of-range access
    if (i >= sharing_edges.size()) {
      throw std::out_of_range("Index out of range");
    }
    return sharing_edges[i];
  }

  void edgeweightcalculation();
  size_t get_sharing_edges_count();

private:
  uint32_t npts;
  uint32_t dim;
  uint32_t max_degree;
  uint32_t start_idx;
  
  uint64_t max_node_len;
  uint64_t nnodes_per_sector;
  uint64_t num_sectors;
  uint64_t disk_index_file_size;

  std::vector<std::vector<float> > data;
  std::vector<std::vector<uint32_t> > neighbors;

  std::set<uint32_t> loaded_pages;

  size_t items_per_page() {     
    return (PAGE_SIZE / SECTOR_SIZE) * nnodes_per_sector; 
  }

  uint32_t get_page_idx(uint32_t data_idx) {
    return data_idx / items_per_page();
  }

  void compute_incoming_edge();
  std::set<uint32_t> find_candidate_neighbors(uint32_t node_id) ;
  uint32_t count_common_incoming_nodes(uint32_t node_id, uint32_t candidate_id) ;
  std::vector<std::vector<uint32_t> > incoming_edges;

  std::vector<std::vector<std::pair<uint32_t,uint32_t> > > sharing_edges;
};



#endif //__DISKINDEX_DATA_HPP__