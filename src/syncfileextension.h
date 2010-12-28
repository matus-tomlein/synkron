#ifndef SYNCFILEEXTENSION_H
#define SYNCFILEEXTENSION_H

class SyncFileExtension
{
public:
    SyncFileExtension();

    int countObsolete() { return num_obsolete; }
    int countNotFound() { return num_not_found; }
    int countDeleted() { return num_deleted; }

private:
    int num_obsolete;
    int num_not_found;
    int num_deleted;
};

#endif // SYNCFILEEXTENSION_H
