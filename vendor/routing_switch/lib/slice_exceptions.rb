class Slice
  # Superclass for not found errors
  class NotFoundError < StandardError; end
  # Slice not found.
  class SliceNotFoundError < NotFoundError; end
  # Port not found.
  class PortNotFoundError < NotFoundError; end
  # MAC address not found.
  class MacAddressNotFoundError < NotFoundError; end

  # Superclass for already exists errors
  class AlreadyExistsError < StandardError; end
  # Slice already exists.
  class SliceAlreadyExistsError < AlreadyExistsError; end
  # Port already exists.
  class PortAlreadyExistsError < AlreadyExistsError; end
  # MAC address already exists.
  class MacAddressAlreadyExistsError < AlreadyExistsError; end
end
