# frozen_string_literal: true

require_relative "ensemble_cast/version"
require_relative "ensemble_cast/ensemble_cast"

module EnsembleCast
  @methods = []
  @enable = false

  TP_CALL = TracePoint.trace(:call) do |tp|
    next unless @methods.any?([tp.self.class, tp.method_id])

    tracepoint_call
    @enable = true
  end

  TP_B_CALL = TracePoint.trace(:b_call) do |tp|
    next unless @enable

    tracepoint_b_call
  end

  TP_RETURN = TracePoint.trace(:return) do |tp|
    next unless @methods.any?([tp.self.class, tp.method_id])

    @enable = false
  end

  class << self
    def add(klass, method)
      @methods << [klass, method]
    end

    def delete(klass, method)
      @methods.detele(klass, method)
    end
  end
end

class Module
  def ensemble(method)
    Inside.add(self, method.name.to_sym)
  end
end
