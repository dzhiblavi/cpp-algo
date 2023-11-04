#pragma once

#include <iterator>
#include <ranges>

namespace test::utility::views {

// template <std::ranges::input_range... Views>
// requires(std::ranges::view<Views> && ...) && (sizeof...(Views) > 0)
// class zip_view : public std::ranges::view_interface<zip_view<Views...>> {
//   template <typename... Its>
//   class zip_iter {
//    public:
//     zip_iter() noexcept = default;
//     zip_iter(const zip_iter&) noexcept = default;
//     zip_iter& operator=(const zip_iter&) noexcept = default;
//     explicit zip_iter(Its... its) noexcept : wrapped_{std::move(its)...} {}

//    private:
//     std::tuple<Its...> wrapped_{};
//   };

//  public:
//   constexpr explicit zip_view(Views... views) noexcept  //
//       : begins_(views.begin()...), ends_(views.end()...) {}

//   auto begin() const noexcept { return begins_; }
//   auto end() const noexcept { return ends_; }

//  private:
//   using iter_t = zip_iter<decltype(std::declval<Views>().begin())...>;

//   iter_t begin_{};
//   iter_t end_{};
// };

// template <std::ranges::viewable_range... Rs>
// constexpr auto zip(Rs&&... rs) {
//   return zip_view<std::views::all_t<decltype((rs))>...>(rs...);
// }

}  // namespace test::utility::views