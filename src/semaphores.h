#ifndef Included_Semaphores_H
#define Included_Semaphores_H

class Semaphore
{
    private:
        int id;
        union semum
        {
          int val;
          struct semid_ds* buf;
          unsigned short* array;
        };

    public:
        Semaphore(int key, int initialValue);
        ~Semaphore();

        void down();
        void up();
};

#endif
