include_directories(
    libopentld/mftracker
    libopentld/tld
    3rdparty/cvblobs
    stereovision
    )

qi_create_bin(stereovision
    main.cpp
    stereovision/auxiliary.cpp
    stereovision/evolution.cpp
    stereovision/imacquisition.cpp
    stereovision/neuralnet.cpp
    stereovision/motion.cpp
    stereovision/robot.cpp
    stereovision/stereotld.cpp
    stereovision/timer.cpp
    stereovision/auxiliary.h
    stereovision/evolution.h
    stereovision/imacquisition.h
    stereovision/motion.h
    stereovision/neuralnet.h
    stereovision/robot.h
    stereovision/stereotld.h
    stereovision/timer.h
    )

qi_use_lib(stereovision
    opentld
    ALCOMMON
    ALVISION
    OPENCV2_CORE
    OPENCV2_HIGHGUI
    OPENCV2_IMGPROC)

