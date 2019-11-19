include_directories(
    libopentld/mftracker
    libopentld/tld
    3rdparty/cvblobs
    imitation)

qi_create_bin(naoimitation
    main.cpp
    imitation/auxiliary.cpp
    imitation/imacquisition.cpp
    imitation/tldinit.cpp
    imitation/motion.cpp
    imitation/trajectory.cpp
    imitation/evolution.cpp
    imitation/auxiliary.h
    imitation/imacquisition.h
    imitation/tldinit.h
    imitation/motion.h
    imitation/trajectory.h
    imitation/evolution.h)

qi_use_lib(naoimitation
    opentld
    ALCOMMON
    ALVISION
    OPENCV2_CORE
    OPENCV2_HIGHGUI
    OPENCV2_IMGPROC)

