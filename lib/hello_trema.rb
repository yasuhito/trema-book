# Hello World!
class HelloTrema < Trema::Controller
  def switch_ready(datapath_id)
    info format('Hello %#x!', datapath_id)
  end
end
