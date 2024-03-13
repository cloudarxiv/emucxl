## emucxl: an emulation framework for CXL-based disaggregated memory applications

This repository contains the source code and scripts to reproduce the experiments in the paper "emucxl: an emulation framework for CXL-based disaggregated memory applications".

#### emucxl setup instructions
emucxl uses the [pond](https://github.com/vtess/Pond) techniques for cxl emulation. 
Refer to the [emucxl setup README](https://github.com/cloudarxiv/pond-emucxl/tree/master/emucxl) for setting up emucxl on a regular 2-socket (2S) system. Or download the virtual appliance from [here](https://drive.google.com/file/d/1) and put it in the `~/images` directory. Run the following command to start the VM.

```bash
    $ bash ./scripts/start_vm.sh
```

#### Reproduce key value store experiments
```bash
bash kv_script.sh > result_kv.txt
```