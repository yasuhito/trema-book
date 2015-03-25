$LOAD_PATH.unshift File.join(__dir__, '..')

class SliceableSwitch < PathManager
  # Slice not found.
  class SliceNotFoundError < StandardError; end
  # Port not found.
  class PortNotFoundError < StandardError; end
end
