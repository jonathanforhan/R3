#pragma once
#include "core/Component.hpp"
#include "api/Types.hpp"
#include "api/Log.hpp"

namespace R3::ec {

class Draw : public Component {
public:
  ~Draw() { LOG(Info, "KILLED"); }
  void initialize() override;
  void tick(double) override;
};

} // namespace R3::ec
