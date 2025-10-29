const char kParentDumpFolderName[] = "dump/";
const char kImgFolderName[]        = "img/";
const char kLogFileName[]          = "log.html";
const char kGraphFileName[]        = "dot.txt";

const size_t fileNameLen = 64;

// Now unused:
// const size_t kTimeDateLen           = sizeof ("2024-01-01_19:19:19");

// const size_t kDumpFolderNameLen     = sizeof ("2024-01-01_19:19:19/");
// const size_t kImgFolderNameLen      = kDumpFolderNameLen + sizeof (kImgFolderName);
// const size_t kImgNameLen            = sizeof ("2024-01-01_19:19:19.svg"); // FIMXE: check extension

// const size_t kLogFolderPathLen      = sizeof (kParentDumpFolderName) + kDumpFolderNameLen;
// const size_t kLogFilePathLen        = kLogFolderPathLen + sizeof (kLogFileName);
// const size_t kGraphFilePathLen      = kLogFolderPathLen + sizeof (kGraphFileName);
// const size_t kImgFolderPathLen      = kLogFolderPathLen + sizeof (kImgFolderName) + kImgNameLen;