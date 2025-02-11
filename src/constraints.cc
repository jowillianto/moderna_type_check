module;
#include <concepts>
export module moderna.type_check:constraints;
namespace moderna::type_check {
  template <typename owner_t, typename viewer_t> struct owner_viewer;

  export template <typename viewer_t>
  concept is_ownable = requires(const viewer_t &viewer) {
    {
      owner_viewer<typename viewer_t::owner_t, viewer_t>::as_owned(viewer)
    } -> std::same_as<typename viewer_t::owner_t>;
  };
  export template <typename owner_t>
  concept is_viewable = requires(const owner_t &owner) {
    {
      owner_viewer<owner_t, typename owner_t::viewer_t>::as_view(owner)
    } -> std::same_as<typename owner_t::viewer_t>;
  };
  export template <is_ownable viewer_t> using owner_t = typename viewer_t::owner_t;
  export template <is_viewable owner_t> using viewer_t = typename owner_t::viewer_t;

  export template <is_viewable owner_t> constexpr viewer_t<owner_t> as_view(const owner_t &owner) {
    return owner_viewer<owner_t, viewer_t<owner_t>>::as_view(owner);
  }
  export template <is_ownable viewer_t> owner_t<viewer_t> constexpr as_owned(const viewer_t &viewer) {
    return owner_viewer<owner_t<viewer_t>, viewer_t>::as_owned(viewer);
  }
  export template <typename owner_t, typename viewer_t>
  concept is_owner_viewer_pair = requires(const owner_t &owner, const viewer_t viewer) {
    std::same_as<typename owner_t::viewer_t, viewer_t>;
    std::same_as<typename viewer_t::owner_t, owner_t>;
  } && is_ownable<viewer_t> && is_viewable<viewer_t>;
  
  export template <typename type_t>
  concept is_clonable = requires (const type_t& type) {
    { type.clone() } -> std::same_as<type_t>;
  };
  export template <typename type_t>
  concept is_clonable_or_copyable = std::is_copy_constructible_v<type_t> || is_clonable<type_t>;
  
  export template <is_clonable_or_copyable type_t> 
  type_t deep_copy( const type_t& type ) {
    if constexpr (std::is_copy_constructible_v<type_t>) {
      return type_t { type };
    }
    else 
      return type.clone();
  }
}