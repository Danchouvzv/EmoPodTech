#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

template <size_t N>
class MovingAverage {
private:
    float values[N];
    int index;
    float sum;
    bool isFull;
    
public:
    MovingAverage() : index(0), sum(0), isFull(false) {
        for (int i = 0; i < N; i++) {
            values[i] = 0;
        }
    }
    
    float addValue(float value) {
        if (isnan(value)) {
            return getAverage();
        }
        
        sum -= values[index];
        values[index] = value;
        sum += value;
        
        index = (index + 1) % N;
        if (index == 0) {
            isFull = true;
        }
        
        return getAverage();
    }
    
    float getAverage() const {
        if (isFull) {
            return sum / N;
        } else if (index > 0) {
            return sum / index;
        } else {
            return 0;
        }
    }
    
    void reset() {
        index = 0;
        sum = 0;
        isFull = false;
        for (int i = 0; i < N; i++) {
            values[i] = 0;
        }
    }
    
    bool isBufferFull() const {
        return isFull;
    }
};

#endif 