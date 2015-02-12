# Hello World!
class HelloTrema < Trema::Controller
  def start(args)
    logger.info "Trema started (args = #{args.inspect})."
  end

  def switch_ready(datapath_id)
    logger.info format('Hello %#x!', datapath_id)
  end
end
