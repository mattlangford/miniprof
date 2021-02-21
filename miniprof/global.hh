#include <string_view>
#include <unordered_map>

namespace miniprof {
struct Config {};

///
/// @brief Main profiler class, used to managed an arbitrary number of scoped profilers
///
class GlobalProfiler {
   public:
    explicit GlobalProfiler(const Config& config);

    ~GlobalProfiler() = default;
    GlobalProfiler(const GlobalProfiler& rhs) = delete;
    GlobalProfiler(GlobalProfiler&& rhs) = delete;
    GlobalProfiler& operator=(const GlobalProfiler& rhs) = delete;
    GlobalProfiler& operator=(GlobalProfiler&& rhs) = delete;

   public:
    struct ScopeIdentifier {
        std::string_view name;
        size_t index;
    };

   public:
    ///
    /// @brief Register the given scope name and retrieve the identifier that can be used to interface with it
    ///
    ScopeIdentifier register_scope(std::string_view name) {
        const size_t scope_index = scopes.size();
        return scopes.emplace_back({name}, scope_index);
    }

   private:
    std::vector<ScopeIdentifier> scopes_;
};
}  // namespace miniprof
