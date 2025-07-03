// Copyright (c) 2022 Graphcore Ltd. All rights reserved.

#include "ControlsForm.hpp"
#include "custom_widgets/rotator.hpp"

#include <PacketSerialisation.h>
#include <cereal/types/string.hpp>

#include <boost/log/trivial.hpp>

#include <iomanip>
#include <fstream>

#include <opencv2/highgui.hpp>

std::uint32_t convertSampleValue(float value) {
  // Maximum of 16 otherwise latency will be too high:
  std::uint32_t sampleCount = value * 16;
  // Must be at least 1:
  return std::max(sampleCount, 1u);
}

ControlsForm::ControlsForm(nanogui::Screen* screen,
                           PacketMuxer& sender,
                           PacketDemuxer& receiver)
    : nanogui::FormHelper(screen),
      saveButton(nullptr),
      preview(nullptr)
{
  window = add_window(nanogui::Vector2i(10, 10), "Control");

  // Scene controls
  add_group("Custom controls");
  auto* rotationWheel = new Rotator(window);
  rotationWheel->set_callback([&](float value) {
    serialise(sender, "value", value);
  });
  add_widget("Value rotator", rotationWheel);
  rotationWheel->set_tooltip("Example custom rotator.");

  // Camera controls
  add_group("Other controls");
  slider = new nanogui::Slider(window);
  slider->set_fixed_width(250);
  slider->set_callback([&](float value) {
    serialise(sender, "value", value);
  });
  slider->set_value(0.f);
  slider->callback()(slider->value());
  add_widget("Value slider", slider);

  // Subscribe to FOV updates from the server (on start-up the server can decide the initial value):
  subs["value"] = receiver.subscribe("value", [this](const ComPacket::ConstSharedPacket& packet) {
    float value = 0.f;
    deserialise(packet, value);
    BOOST_LOG_TRIVIAL(trace) << "Received value update: " << value;
    slider->set_value(value);
  });

  // Info/stats/status:
  add_group("Status");
  auto progress = new nanogui::ProgressBar(window);
  add_widget("Progress", progress);

  add_button("Stop", [screen, &sender]() {
    serialise(sender, "stop", true);
    screen->set_visible(false);
  })->set_tooltip("Stop the remote application.");

  // Make a subscriber to receive progress updates:
  // (the progress pointer needs to be captured by value).
  subs["progress"] = receiver.subscribe("progress", [progress](const ComPacket::ConstSharedPacket& packet) {
    float progressValue = 0.f;
    deserialise(packet, progressValue);
    progress->set_value(progressValue);
  });

  add_group("Info/Stats");
  bitRateText = new nanogui::TextBox(window, "-");
  bitRateText->set_editable(false);
  bitRateText->set_units("Mbps");
  bitRateText->set_alignment(nanogui::TextBox::Alignment::Right);
  add_widget("Video rate:", bitRateText);

  frameRateText = new nanogui::TextBox(window, "-");
  frameRateText->set_editable(false);
  frameRateText->set_units("Frames/sec");
  frameRateText->set_alignment(nanogui::TextBox::Alignment::Right);
  add_widget("Frame rate:", frameRateText);

  add_group("File Manager");
  saveButton = add_button("Save image", [this]() {
    saveImage();
  });
  saveButton->set_tooltip("Save preview image locally.");

  subs["state"] = receiver.subscribe("state",
    [&](const ComPacket::ConstSharedPacket& packet) {
      ServerState state;
      deserialise(packet, state);
      BOOST_LOG_TRIVIAL(info) << "Received full state update: " << state.toString();
        // Update UI controls to match the server state:
        slider->set_value(state.value / 10.0f); // Convert back from 0-10 range to 0-1
    }
  );
}

void ControlsForm::set_position(const nanogui::Vector2i& pos) {
  window->set_position(pos);
}

void ControlsForm::saveImage() const {
  if (preview) {
    const std::string fn = "preview.png";
    BOOST_LOG_TRIVIAL(info) << "Saving image as " << fn;
    cv::imwrite(fn, preview->getImage());
  }
}
