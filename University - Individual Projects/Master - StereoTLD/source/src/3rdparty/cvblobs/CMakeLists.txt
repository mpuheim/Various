qi_create_lib(cvblobs
    blob.cpp
    BlobContour.cpp
    BlobOperators.cpp
    BlobProperties.cpp
    BlobResult.cpp
    ComponentLabeling.cpp
    blob.h
    BlobContour.h
    BlobLibraryConfiguration.h
    BlobOperators.h
    BlobProperties.h
    BlobResult.h
    ComponentLabeling.h)
    
qi_use_lib(cvblobs
    OPENCV2_CORE
    OPENCV2_HIGHGUI
    OPENCV2_IMGPROC
    OPENCV2_VIDEO
    OPENCV2_CALIB3D
    OPENCV2_FEATURES2D
    OPENCV2_ML)
    
qi_stage_lib(cvblobs)
