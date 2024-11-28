#include "diskindex_data.hpp"
#include <fstream>
#include <iostream>
#include <memory.h>
#include "config.hpp"


bool DiskIndexData::Load(const std::string& filename) {
  std::ifstream file(filename,std::ios::binary);

  if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      return false;
  }

  char sector_buffer[SECTOR_SIZE];

  file.read(sector_buffer, SECTOR_SIZE);

  uint32_t dummy0 = *(((uint32_t*)sector_buffer)+0); //should be 9;
  uint32_t dummy1 = *(((uint32_t*)sector_buffer)+1); //should be 1;

  npts = *(((uint64_t*)sector_buffer)+1); 
  dim = *(((uint64_t*)sector_buffer)+2);
  start_idx = *(((uint64_t*)sector_buffer)+3);
  max_node_len = *(((uint64_t*)sector_buffer)+4);
  nnodes_per_sector = *(((uint64_t*)sector_buffer)+5);

  max_degree = ((max_node_len - dim*sizeof(float)) / sizeof(uint32_t)) - 1;

  disk_index_file_size = *(((uint64_t*)sector_buffer)+9);

  num_sectors = (disk_index_file_size / SECTOR_SIZE) - 1;

  for (size_t pi = 0; pi < num_sectors; pi++) {
    file.read(sector_buffer, SECTOR_SIZE);

    for (size_t offset_i = 0; offset_i < nnodes_per_sector; offset_i++) {
      std::vector<float> tempVec;
      tempVec.resize(dim);
      char* start_ptr = sector_buffer + max_node_len*offset_i;
      memcpy(&tempVec[0],  start_ptr,dim * sizeof(float));
      data.push_back(tempVec);

      uint32_t n_neighbors;
      memcpy(&n_neighbors,  start_ptr + dim*sizeof(float), sizeof(uint32_t));

      std::vector<uint32_t> tempNeighbors;
      tempNeighbors.resize(n_neighbors);
      memcpy(&tempNeighbors[0],  start_ptr + dim*sizeof(float) + sizeof(uint32_t),n_neighbors * sizeof(uint32_t));
      neighbors.push_back(tempNeighbors);  

      if(data.size() >= npts) break;
    }

  }
  return true;
}

void DiskIndexData::compute_incoming_edge() {
  incoming_edges.resize(neighbors.size());
  for(uint32_t vi = 0; vi < neighbors.size(); vi++) {
    for(auto neighbor_id : neighbors[vi]) {
      incoming_edges[neighbor_id].push_back(vi);
    }    
  }
}

std::set<uint32_t> DiskIndexData::find_candidate_neighbors(uint32_t node_id) {
  std::set <uint32_t> res;
  
  for(auto incoming_id : incoming_edges[node_id]) {
    auto outgoing_edges_of_incoming_node = neighbors[incoming_id];
    res.insert(outgoing_edges_of_incoming_node.begin(), outgoing_edges_of_incoming_node.end());    
  }

  return res;  
}

uint32_t DiskIndexData::count_common_incoming_nodes(uint32_t node_id, uint32_t candidate_id)  {
  // Counter for the number of common elements
  std::vector<uint32_t> commonElements;

  // Find intersection without storing the results
  std::set_intersection(incoming_edges[node_id].begin(), incoming_edges[node_id].end(),
                        incoming_edges[candidate_id].begin(), incoming_edges[candidate_id].end(),
                        std::back_inserter(commonElements));
  return commonElements.size();
}


void DiskIndexData::edgeweightcalculation() {
  compute_incoming_edge();

  sharing_edges.resize(getSize());

  for(int vi = 0; vi < getSize(); vi++) {
    auto candidate_neighbors = find_candidate_neighbors(vi);
    
    for (auto candidate_neighbor_id : candidate_neighbors) {

      if( vi == candidate_neighbor_id ) continue;
      
      uint32_t n_common_incoming_nodes =  count_common_incoming_nodes(vi,candidate_neighbor_id);

      sharing_edges[vi].push_back(std::make_pair(candidate_neighbor_id,n_common_incoming_nodes));
    }
  }
}

size_t DiskIndexData::get_sharing_edges_count() {
  size_t totalCount = 0;
  for( const auto& sharing_edge: sharing_edges) {
    totalCount += sharing_edge.size();
  }
  return totalCount;
}

