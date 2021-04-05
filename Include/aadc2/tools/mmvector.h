//
// Created by cbelous on 09/10/2019.
//

#ifndef AADC_MMVECTOR_H
#define AADC_MMVECTOR_H

template<typename mmType>
class mmVector {
public:
    mmVector(const std::size_t& size)
            : m_size(size)
    {
        m_alloc = calloc(size+1, sizeof(mmType));
        m_v = align(m_alloc);
    }
    mmVector(const std::size_t& size, const mmType& v)
            : m_size(size)
    {
        m_alloc = calloc(size+1, sizeof(mmType));
        m_v = align(m_alloc);
        std::fill_n(m_v, m_size, v);
    }
    mmVector()
            : m_size(0)
            , m_alloc(0)
            , m_v(0)
    {}
    mmVector(const mmVector<mmType>& other)
            : m_size(other.m_size)
    {
        m_alloc = calloc(m_size+1, sizeof(mmType));
        m_v = align(m_alloc);
        std::copy(other.m_v, other.m_v + m_size, m_v);
    }
    ~mmVector() {
        if (m_alloc) free(m_alloc);
    }
    void resize(std::size_t new_size) {
        if (m_alloc && m_size >= new_size) {
            m_size = new_size;
            return ;
        }
        void* new_alloc = calloc(new_size+1, sizeof(mmType));
        mmType* new_v = align(new_alloc);
        if (m_alloc && m_size > 0) {
            std::copy_n(m_v, m_size, new_v);
            free(m_alloc);
        }
        m_v = new_v;
        m_alloc = new_alloc;
        m_size = new_size;
    }
    mmType* begin() { return m_v; }
    mmType* end() { return m_v + m_size; }
    std::size_t size() const { return m_size; }

    mmType& front() { return *m_v; }
    const mmType& front() const { return *m_v; }

    mmType& back() { return *(m_v + m_size - 1); }
    const mmType& back() const { return *(m_v + m_size - 1); }

    mmType& operator [](const std::size_t& i) { return m_v[i]; }

    const mmType& operator [](const std::size_t& i) const { return m_v[i]; }

    mmVector& operator = (const mmVector<mmType>& other)

    {
        if (m_alloc) free(m_alloc);
        m_size=other.m_size;
        m_alloc = calloc(m_size+1, sizeof(mmType));
        m_v = align(m_alloc);
        std::copy(other.m_v, other.m_v + m_size, m_v);
        return *this;
    }
private:
    static mmType* align(const void* p) {
        uint64_t pp = (uint64_t)p;
        pp += sizeof(mmType) - (pp % sizeof(mmType));
        return (mmType*)pp;
    }

private:
    std::size_t m_size;
    void* m_alloc;
    mmType* m_v;
};

#endif //AADC_MMVECTOR_H
