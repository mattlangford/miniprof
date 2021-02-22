namespace miniprof {
///
/// @brief RAII helper for managing scoped profiling. An event is registered at construction and ended at deconstruction
///
class ScopedProfiler {
public:
    Scoped() ~Scoped() private:
};
}  // namespace miniprof

#define PROFILE(name)
