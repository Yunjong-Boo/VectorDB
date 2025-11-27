# Full-FPGA Vector Search Accelerator with HBM-Aware NoC

This project implements a full-FPGA vector search accelerator that maps the entire ANN search pipeline onto the FPGA fabric.
Multiple hardware Navigator modules cooperate over an HBM-aware NoC to achieve high QPS on large-scale datasets.

* * *

## Features

  * Modular Search Pipeline on FPGA  
    Distance computation, priority-queue–based candidate selection, and HBM access are all implemented as dedicated hardware modules.

  * Hardware Navigator for ANN Search  
    The software search algorithm is translated into a Navigator FSM on FPGA, enabling deterministic, low-latency traversal of the index graph.

  * Multi-Query Execution with Multi-Navigator  
    Multiple Navigator instances process independent queries in parallel, scaling throughput linearly with the number of Navigators.

  * HBM-Aware NoC between Navigators and Multi-Channel HBM  
    A lightweight FPGA-friendly NoC connects Navigators to HBM channels, sustaining random, many-to-many traffic without throughput collapse.

* * *

## Implementation Details

### Search Modules

  * Distance Engine  
    - Receives query vector beats from the NoC, aligns them per context, and reads matching resident vectors from on-chip memory.  
    - Computes lane-wise distance, accumulates over all beats of a vector in a pipelined fashion, and outputs the final L2-squared distance per job/context.

  * Priority Queue Module  
    - BRAM-backed min–max heap that stores `(key, id)` pairs
    - Used as a hardware-friendly priority queue for managing candidate lists and top-k results in the search pipeline.

  * HBM Interface  
    - Collects load requests from the search pipeline (Navigator IDs + addresses), queues them, and issues AXI4 read bursts to HBM with proper `arlen/arsize` selection for vector and neighbor packets.  
    - Return `rdata` beats with the original Navigator context, repack them into fixed-size packets, and stream them back into the NoC as `{dest, data, last}` without stalling the HBM channels.
    
  * Navigator Module  
    Encodes the ANN search algorithm as a finite-state machine:
    - issues neighbor-list and vector fetches via the NoC  
    - updates candidate pools and top-k results  
    - terminates when the search budget visited nodes is reached  

### Multi-Query Execution

  * Multi-Navigator Organization  
    Instantiates `N_nav` Navigator pipelines, each bound to one query at a time.
    A simple query manager assigns queries to idle Navigators.

  * Backpressure and Flow Control  
    Each Navigator observes NoC and HBM-ready signals and throttles its own requests when congestion occurs, ensuring fairness across queries.

### HBM-Aware NoC

  * Topology  
    FPGA-friendly indirect-grid–style NoC between `N_nav` Navigators (sources) and `N_ch` HBM channels (sinks).  
    The mid-plane is sparsified and assisted by small “miniswitch” blocks so that random many-to-many traffic can be sustained with fewer routers and queues.

  * Routing & Deadlock Freedom  
    Deterministic XY routing (X then Y) with packet-level arbitration ensures deadlock-free operation and preserves in-order delivery per (Navigator, HBM-channel) flow.

  * Queues and Arbitration  
    - Per-output or per-destination VOQs to avoid head-of-line blocking under random destinations  
    - Round-robin arbitration at routers and miniswitches  
    This combination keeps throughput close to the per-channel service limit even when requests show high entropy.

  * FPGA-Aware Floorplanning  
    Routers and miniswitches are placed near Navigator and HBM endpoints, with short, pipelined links across SLR boundaries to sustain the target clock frequency.

* * *

## Architectural Summary

The full-FPGA accelerator is organized as follows:

  * Query Manager  
    - Tracks the state of each Navigator (idle / busy)  
    - Assigns new queries to idle Navigators and collects their final results  

  * Navigator Array  
    - `N_nav` parallel Navigator pipelines  
    - Each pipeline internally chains Distance Engine, Priority Queue, and control FSM

  * HBM-Aware NoC Fabric  
    - Connects each Navigator to all HBM channels  
    - Provides uniform bandwidth under high-entropy, multi-source / multi-sink traffic

  * HBM Channel Banks  
    - Store index graph (neighbor lists) and vectors partitioned across channels  
    - Layout is aligned with NoC routing to balance load across channels

* * *
