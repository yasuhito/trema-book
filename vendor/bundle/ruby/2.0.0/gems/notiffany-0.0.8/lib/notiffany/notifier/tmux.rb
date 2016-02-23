require "notiffany/notifier/base"
require "shellany/sheller"

# TODO: this probably deserves a gem of it's own
module Notiffany
  class Notifier
    # Changes the color of the Tmux status bar and optionally
    # shows messages in the status bar.
    class Tmux < Base
      @session = nil

      DEFAULTS = {
        tmux_environment:       "TMUX",
        success:                "green",
        failed:                 "red",
        pending:                "yellow",
        default:                "green",
        timeout:                5,
        display_message:        false,
        default_message_format: "%s - %s",
        default_message_color:  "white",
        display_on_all_clients: false,
        display_title:          false,
        default_title_format:   "%s - %s",
        line_separator:         " - ",
        change_color:           true,
        color_location:         "status-left-bg"
      }

      class Client
        CLIENT = "tmux"

        class << self
          def version
            Float(_capture("-V")[/\d+\.\d+/])
          end

          def _capture(*args)
            Shellany::Sheller.stdout(([CLIENT] + args).join(" "))
          end

          def _run(*args)
            Shellany::Sheller.run(([CLIENT] + args).join(" "))
          end
        end

        def initialize(client)
          @client = client
        end

        def clients
          return [@client] unless @client == :all
          ttys = _capture("list-clients", "-F", "'\#{client_tty}'")
          ttys = ttys.split(/\n/)

          # if user is running 'tmux -C' remove this client from list
          ttys.delete("(null)")
          ttys
        end

        def set(key, value)
          clients.each do |client|
            args = client ? ["-t", client.strip] : nil
            _run("set", "-q", *args, key, value)
          end
        end

        def display_message(message)
          clients.each do |client|
            args = ["-c", client.strip] if client
            # TODO: should properly escape message here
            _run("display", *args, "'#{message}'")
          end
        end

        def unset(key, value)
          clients.each do |client|
            args = client ? ["-t", client.strip] : []
            if value
              _run("set", "-q", *args, key, value)
            else
              _run("set", "-q", "-u", *args, key)
            end
          end
        end

        def parse_options
          output = _capture("show", "-t", @client)
          Hash[output.lines.map { |line| _parse_option(line) }]
        end

        def message_fg=(color)
          set("message-fg", color)
        end

        def message_bg=(color)
          set("message-bg", color)
        end

        def display_time=(time)
          set("display-time", time)
        end

        def title=(string)
          # TODO: properly escape?
          set("set-titles-string", "'#{string}'")
        end

        private

        def _run(*args)
          self.class._run(*args)
        end

        def _capture(*args)
          self.class._capture(*args)
        end

        def _parse_option(line)
          line.partition(" ").map(&:strip).reject(&:empty?)
        end
      end

      class Session
        def initialize
          @options_store = {}

          # NOTE: we are reading the settings of all clients
          # - regardless of the :display_on_all_clients option

          # Ideally, this should be done incrementally (e.g. if we start with
          # "current" client and then override the :display_on_all_clients to
          # true, only then the option store should be updated to contain
          # settings of all clients
          Client.new(:all).clients.each do |client|
            @options_store[client] = {
              "status-left-bg"  => nil,
              "status-right-bg" => nil,
              "status-left-fg"  => nil,
              "status-right-fg" => nil,
              "message-bg"      => nil,
              "message-fg"      => nil,
              "display-time"    => nil
            }.merge(Client.new(client).parse_options)
          end
        end

        def close
          @options_store.each do |client, options|
            options.each do |key, value|
              Client.new(client).unset(key, value)
            end
          end
          @options_store = nil
        end
      end

      class Error < RuntimeError
      end

      ERROR_NOT_INSIDE_TMUX = ":tmux notifier is only available inside a "\
        "TMux session."

      ERROR_ANCIENT_TMUX = "Your tmux version is way too old (%s)!"

      # Notification starting, save the current Tmux settings
      # and quiet the Tmux output.
      #
      def turn_on
        self.class._start_session
      end

      # Notification stopping. Restore the previous Tmux state
      # if available (existing options are restored, new options
      # are unset) and unquiet the Tmux output.
      #
      def turn_off
        self.class._end_session
      end

      private

      def _gem_name
        nil
      end

      def _check_available(opts = {})
        @session ||= nil # to avoid unitialized error
        fail "PREVIOUS TMUX SESSION NOT CLEARED!" if @session

        var_name = opts[:tmux_environment]
        fail Error, ERROR_NOT_INSIDE_TMUX unless ENV.key?(var_name)

        version = Client.version
        fail Error, format(ERROR_ANCIENT_TMUX, version) if version < 1.7

        true
      rescue Error => e
        fail UnavailableError, e.message
      end

      # Shows a system notification.
      #
      # By default, the Tmux notifier only makes
      # use of a color based notification, changing the background color of the
      # `color_location` to the color defined in either the `success`,
      # `failed`, `pending` or `default`, depending on the notification type.
      #
      # You may enable an extra explicit message by setting `display_message`
      # to true, and may further disable the colorization by setting
      # `change_color` to false.
      #
      # @param [String] message the notification message
      # @param [Hash] options additional notification library options
      # @option options [String] type the notification type. Either 'success',
      #   'pending', 'failed' or 'notify'
      # @option options [String] message the notification message body
      # @option options [String] image the path to the notification image
      # @option options [Boolean] change_color whether to show a color
      #   notification
      # @option options [String,Array] color_location the location where to draw
      #   the color notification
      # @option options [Boolean] display_message whether to display a message
      #   or not
      # @option options [Boolean] display_on_all_clients whether to display a
      #   message on all tmux clients or not
      #
      def _perform_notify(message, options = {})
        change_color = options[:change_color]
        locations = Array(options[:color_location])
        display_the_title = options[:display_title]
        display_message = options[:display_message]
        type  = options[:type].to_s
        title = options[:title]

        if change_color
          color = _tmux_color(type, options)
          locations.each do |location|
            Client.new(client(options)).set(location, color)
          end
        end

        _display_title(type, title, message, options) if display_the_title

        return unless display_message
        _display_message(type, title, message, options)
      end

      # Displays a message in the title bar of the terminal.
      #
      # @param [String] title the notification title
      # @param [String] message the notification message body
      # @param [Hash] options additional notification library options
      # @option options [String] success_message_format a string to use as
      #   formatter for the success message.
      # @option options [String] failed_message_format a string to use as
      #   formatter for the failed message.
      # @option options [String] pending_message_format a string to use as
      #   formatter for the pending message.
      # @option options [String] default_message_format a string to use as
      #   formatter when no format per type is defined.
      #
      def _display_title(type, title, message, options = {})
        format = "#{type}_title_format".to_sym
        default_title_format = options[:default_title_format]
        title_format   = options.fetch(format, default_title_format)
        teaser_message = message.split("\n").first
        display_title  = title_format % [title, teaser_message]

        Client.new(client(options)).title = display_title
      end

      # Displays a message in the status bar of tmux.
      #
      # @param [String] type the notification type. Either 'success',
      #   'pending', 'failed' or 'notify'
      # @param [String] title the notification title
      # @param [String] message the notification message body
      # @param [Hash] options additional notification library options
      # @option options [Integer] timeout the amount of seconds to show the
      #   message in the status bar
      # @option options [String] success_message_format a string to use as
      #   formatter for the success message.
      # @option options [String] failed_message_format a string to use as
      #   formatter for the failed message.
      # @option options [String] pending_message_format a string to use as
      #   formatter for the pending message.
      # @option options [String] default_message_format a string to use as
      #   formatter when no format per type is defined.
      # @option options [String] success_message_color the success notification
      #   foreground color name.
      # @option options [String] failed_message_color the failed notification
      #   foreground color name.
      # @option options [String] pending_message_color the pending notification
      #   foreground color name.
      # @option options [String] default_message_color a notification
      #   foreground color to use when no color per type is defined.
      # @option options [String] line_separator a string to use instead of a
      #   line-break.
      #
      def _display_message(type, title, message, opts = {})
        default_format = opts[:default_message_format]
        default_color = opts[:default_message_color]
        display_time = opts[:timeout]
        separator = opts[:line_separator]

        format = "#{type}_message_format".to_sym
        message_format = opts.fetch(format, default_format)

        color = "#{type}_message_color".to_sym
        message_color = opts.fetch(color, default_color)

        color = _tmux_color(type, opts)
        formatted_message = message.split("\n").join(separator)
        msg = message_format % [title, formatted_message]

        cl = Client.new(client(opts))
        cl.display_time = display_time * 1000
        cl.message_fg = message_color
        cl.message_bg = color
        cl.display_message(msg)
      end

      # Get the Tmux color for the notification type.
      # You can configure your own color by overwriting the defaults.
      #
      # @param [String] type the notification type
      # @return [String] the name of the emacs color
      #
      def _tmux_color(type, opts = {})
        type = type.to_sym
        opts[type] || opts[:default]
      end

      def self._start_session
        fail "Already turned on!" if @session
        @session = Session.new
      end

      def self._end_session
        fail "Already turned off!" unless @session || nil
        @session.close
        @session = nil
      end

      def self._session
        @session
      end

      def client(options)
        options[:display_on_all_clients] ? :all : nil
      end
    end
  end
end
