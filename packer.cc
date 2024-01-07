/*
 *┌──────────┬───────────┬──────────────────┬─┬────────────────────────────┐
 *│          │           │                  │ ├──────────────────────────┼─┤
 *│ camera_id│ rev_flag=3│contour_extend_num│ │                          │ │
 *│ (uint16) │ (uint16)  │    (uint32)      │ │                          │ │
 *│          │           │                  │ ├──────────────────────────┼─┤
 *└──────────┴───────────┴──────────────────┴─┴────────────────────────────┘
 *                                           ◄──  contour_extend_num -1  ──►
 */
#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

class ContourMsg
{
public:
    // types
    using point_type              = uint64_t;
    using label_type              = uint32_t;
    using camera_id_type          = uint32_t;
    using rev_flag_type           = uint32_t;
    using contour_extend_num_type = uint32_t;
    using pts_num_type            = uint32_t;

#pragma pack(push, 1)
    struct Point
    {
        Point() = default;
        Point(float x, float y, point_type type) : x_{x}, y_{y}, type_{type} {}
        float      x_;
        float      y_;
        point_type type_;
    };
#pragma pack(pop)

    using img_pts_type = std::vector<Point>;

    struct ImgPtsInfo
    {
        ImgPtsInfo() = default;
        ImgPtsInfo(label_type label, img_pts_type&& pts_out_img,
                   img_pts_type&& pts_above_vp)
            : label_{label},
              pts_out_img_{pts_out_img},
              pts_above_vp_{pts_above_vp}
        {
        }
        label_type   label_{0U};
        img_pts_type pts_out_img_;
        img_pts_type pts_above_vp_;
    };

    using img_pts_infos_type = std::vector<ImgPtsInfo>;

    struct Contour
    {
        camera_id_type     camera_id_{0U};
        rev_flag_type      rev_flag_{0U};
        img_pts_infos_type img_pts_infos_;
    };

public:
    // ctor
    ContourMsg()  = delete;
    ~ContourMsg() = delete;

    // funcs

    // static funcs
    // static ContourMsg GetContourMsgFromData(std::vector<uint8_t> const&
    // data);
    static bool GetContourFromData(std::vector<uint8_t> const& data, Contour& res);

private:
    static bool GetPointFromData(std::vector<uint8_t> const& data, std::size_t& offset,
                                 rev_flag_type const flat, img_pts_type& res);
    static bool GetImgPtsInfoFromData(std::vector<uint8_t> const& data,
                                      std::size_t& offset, rev_flag_type const flag,
                                      img_pts_infos_type& res);
};

enum class ContourLimit
{
    contour_extend_num_max = 100,
    img_pts_num_max        = 300,
    camera_id_max          = 100,
    rev_flag_max           = 10,
    data_size_max          = 8,
};

enum class ContourRevFlag : ContourMsg::rev_flag_type
{
    f1 = 1U,
    f2 = 2U,
    f3 = 3U,
};

// for pos error
constexpr std::size_t g_npos{SIZE_MAX};

template <typename T>
static std::size_t xcopy_n(T& dst, std::vector<uint8_t> const& data, std::ptrdiff_t pos)
{
    if (data.size() < (static_cast<std::size_t>(pos) + sizeof(T) - 1U)) return g_npos;
    memcpy(&dst, data.data() + pos, sizeof(T));
    return pos + sizeof(T);
}

#define XCOPY(dst, d, p) (p) = xcopy_n(dst, (d), (p))

#define CHECK_POS(p, str)                                        \
    do {                                                         \
        if ((p) == g_npos) {                                     \
            std::cout << "get " << #str << " from data error "   \
                      << "[ data lenght less than required ]\n"; \
            return false;                                        \
        }                                                        \
    } while (0)

#define CHECK_POS_VALID(p, v, g)                          \
    do {                                                  \
        CHECK_POS((p), (v));                              \
        if ((v) >= static_cast<uint32_t>(g)) {            \
            std::cout << "get " << #v << " is invalid\n"; \
            return false;                                 \
        }                                                 \
    } while (0)

bool ContourMsg::GetPointFromData(const std::vector<uint8_t>& data, std::size_t& offset,
                                  rev_flag_type const flag, img_pts_type& res)
{
    float      x{0.0f};
    float      y{0.0f};
    point_type type{0U};
    auto       pos{xcopy_n(x, data, offset)};
    CHECK_POS(pos, x);
    XCOPY(y, data, pos);
    CHECK_POS(pos, y);
    if (static_cast<rev_flag_type>(ContourRevFlag::f3) >= flag) {
        XCOPY(type, data, pos);
        CHECK_POS(pos, type);
    }
    offset = pos;  // keep pos
    res.emplace_back(x, y, type);
    return true;
}

#define GET_POINTS(n, d, p, f, r)                        \
    do {                                                 \
        for (uint32_t i = 0; i < (n); i++) {             \
            if (!GetPointFromData((d), (p), (f), (r))) { \
                return false;                            \
            }                                            \
        }                                                \
    } while (0)

bool ContourMsg::GetImgPtsInfoFromData(std::vector<uint8_t> const& data,
                                       std::size_t& offset, rev_flag_type const flag,
                                       img_pts_infos_type& res)
{
    label_type   label{0U};
    pts_num_type pts_img_num{0U};
    pts_num_type pts_above_vp_num{0U};
    pts_num_type pts_vr_num{0U};
    img_pts_type pts_out_img;
    img_pts_type pts_above_vp;
    auto         pos{xcopy_n(label, data, offset)};
    CHECK_POS(pos, label);
    if (flag > 0U) {
        XCOPY(pts_img_num, data, pos);
        CHECK_POS(pos, pts_img_num);
        XCOPY(pts_above_vp_num, data, pos);
        CHECK_POS(pos, pts_above_vp_num);
    }
    if (flag == static_cast<rev_flag_type>(ContourRevFlag::f2)) {
        XCOPY(pts_vr_num, data, pos);
        CHECK_POS(pos, pts_vr_num);
    }
    GET_POINTS(pts_img_num, data, pos, flag, pts_out_img);
    GET_POINTS(pts_above_vp_num, data, pos, flag, pts_above_vp);
    if (flag == static_cast<rev_flag_type>(ContourRevFlag::f2)) {
        img_pts_type vr;
        GET_POINTS(pts_vr_num, data, pos, flag, vr);
    }
    offset = pos;
    res.emplace_back(label, std::move(pts_out_img), std::move(pts_above_vp));
    return true;
}

bool ContourMsg::GetContourFromData(std::vector<uint8_t> const& data, Contour& res)
{
    if (data.empty()) {
        return false;
    }
    camera_id_type          cid{0U};
    rev_flag_type           flag{0U};
    contour_extend_num_type contour_extend_num{0U};
    img_pts_infos_type      infos;
    auto                    pos{xcopy_n(cid, data, 0)};

    CHECK_POS_VALID(pos, cid, ContourLimit::camera_id_max);
    XCOPY(flag, data, pos);
    CHECK_POS_VALID(pos, flag, ContourLimit::rev_flag_max);
    XCOPY(contour_extend_num, data, pos);
    CHECK_POS_VALID(pos, contour_extend_num, ContourLimit::contour_extend_num_max);
    for (contour_extend_num_type i = 0U; i < contour_extend_num; i++) {
        if (!GetImgPtsInfoFromData(data, pos, flag, infos)) {
            return false;
        }
    }
    res.camera_id_     = cid;
    res.rev_flag_      = flag;
    res.img_pts_infos_ = std::move(infos);
    return true;
}

int main(int argc, char* argv[])
{
    float   x{2.34};
    float   s{0};
    float   y{3.443};
    int32_t type{998};

    std::vector<uint8_t> data(sizeof(float) * 2U + sizeof(type));

    std::cout << data.size() << '\n';

    std::cout << sizeof(x) << '\n';
    std::cout << "x=" << x << " s=" << s << '\n';

    memcpy(data.data(), &x, sizeof(x));
    memcpy(data.data() + sizeof(x), &y, sizeof(y));
    memcpy(data.data() + sizeof(x) + sizeof(y), &type, sizeof(type));

    auto n{xcopy_n(s, data, 0)};

    std::cout << "sizeof(s)=" << sizeof(s) << " data.size()=" << data.size() << '\n';
    std::cout << "n=" << n << " x=" << x << " s=" << s << '\n';

    return 0;
}
