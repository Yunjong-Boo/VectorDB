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
    A lightweight FPGA-friendly NoC connects Navigators to HBM channels, sustaining random, many-to-many traffic without throughput collapse. :contentReference[oaicite:0]{index=0}

  * Dataset-Validated Performance (SIFT / SPACEV)  
    On SIFT and SPACEV datasets, the full-FPGA design achieves up to XXX K QPS while maintaining target recall.

* * *

## Implementation Details

### Search Modules

  * Distance Engine  
    Computes vector distances with a deeply pipelined fixed-point or low-precision arithmetic unit.
    The design is dimension-parameterizable (e.g., 128 / 256-D) and matched to HBM burst size.

  * Priority Queue Module  
    Maintains candidate nodes and top-k results using a hardware-friendly heap or partial sorter.
    Supports configurable `k` and search-list size.

  * HBM Interface  
    Wraps the vendor HBM IP and exposes a clean request–response interface (ID, address, length).
    Aggregates burst accesses and aligns them to HBM channel/bank boundaries.

  * Navigator Module  
    Encodes the ANN search algorithm as a finite-state machine:
    - issues neighbor-list and vector fetches via the NoC  
    - updates candidate pools and top-k results  
    - terminates when the search budget (steps / visited nodes) is reached  

### Multi-Query Execution

  * Multi-Navigator Organization  
    Instantiates `N_nav` Navigator pipelines, each bound to one query at a time.
    A simple dispatcher assigns incoming queries to idle Navigators.

  * Backpressure and Flow Control  
    Each Navigator observes NoC and HBM-ready signals and throttles its own requests when congestion occurs, ensuring fairness across queries.

### HBM-Aware NoC

  * Topology  
    FPGA-friendly indirect-grid–style NoC between `N_nav` Navigators (sources) and `N_ch` HBM channels (sinks).  
    The mid-plane is sparsified and assisted by small “miniswitch” blocks so that random many-to-many traffic can be sustained with fewer routers and queues. :contentReference[oaicite:1]{index=1}

  * Routing & Deadlock Freedom  
    Deterministic XY routing (X then Y) with packet-level arbitration ensures deadlock-free operation and preserves in-order delivery per (Navigator, HBM-channel) flow. :contentReference[oaicite:2]{index=2}

  * Queues and Arbitration  
    - Per-output or per-destination VOQs to avoid head-of-line blocking under random destinations  
    - Round-robin arbitration at routers and miniswitches  
    This combination keeps throughput close to the per-channel service limit even when requests show high entropy. :contentReference[oaicite:3]{index=3}

  * FPGA-Aware Floorplanning  
    Routers and miniswitches are placed near Navigator and HBM endpoints, with short, pipelined links across SLR boundaries to sustain the target clock frequency. :contentReference[oaicite:4]{index=4}

* * *

## Architectural Summary

The full-FPGA accelerator is organized as follows:

  * Host Interface  
    - Receives batched queries from the host and writes them into on-board buffers  
    - Reads back top-k results after FPGA-side search completes  

  * Query Dispatcher  
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
