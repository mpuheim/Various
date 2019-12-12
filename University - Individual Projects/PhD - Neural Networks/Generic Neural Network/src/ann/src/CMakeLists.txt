include_directories(
    ann
    )

qi_create_bin(ann_start
    main.cpp
    ann/auxiliary.cpp
    ann/neuralnet.cpp
    ann/timer.cpp
    ann/auxiliary.h
    ann/neuralnet.h
    ann/timer.h
    )

# qi_use_lib()

