include_directories(imacq
    mftracker
    tld
    ../3rdparty/cvblobs)

qi_create_lib(opentld
    mftracker/BB.cpp
    mftracker/BBPredict.cpp
    mftracker/FBTrack.cpp
    mftracker/Lk.cpp
    mftracker/Median.cpp
    tld/Clustering.cpp
    tld/DetectionResult.cpp
    tld/DetectorCascade.cpp
    tld/EnsembleClassifier.cpp
    tld/ForegroundDetector.cpp
    tld/MedianFlowTracker.cpp
    tld/NNClassifier.cpp
    tld/TLD.cpp
    tld/TLDUtil.cpp
    tld/VarianceFilter.cpp
    mftracker/BB.h
    mftracker/BBPredict.h
    mftracker/FBTrack.h
    mftracker/Lk.h
    mftracker/Median.h
    tld/Clustering.h
    tld/DetectionResult.h
    tld/DetectorCascade.h
    tld/EnsembleClassifier.h
    tld/ForegroundDetector.h
    tld/IntegralImage.h
    tld/MedianFlowTracker.h
    tld/NNClassifier.h
    tld/NormalizedPatch.h
    tld/TLD.h
    tld/TLDUtil.h
    tld/VarianceFilter.h)
    
qi_use_lib(opentld
    cvblobs
    OPENCV2_CORE
    OPENCV2_HIGHGUI
    OPENCV2_IMGPROC
    OPENCV2_VIDEO
    OPENCV2_CALIB3D
    OPENCV2_FEATURES2D
    OPENCV2_ML)
    
qi_stage_lib(opentld)

qi_install_header(tld/TLD.h
  SUBFOLDER tld)

