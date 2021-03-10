#pragma once

#include <array>
#include <functional>
#include <type_traits>

namespace fsm
{
	namespace detail
	{
		template <typename T>
		struct is_bool : public std::is_same<T, bool> {
		};
	}

	template <typename A, typename S, size_t A_num, size_t S_num>
	class FSM {
	private:
		static constexpr bool check_state_type()
		{
			return (std::is_integral<S>::value || std::is_enum<S>::value) && !detail::is_bool<S>::value;
		}

		static constexpr bool check_symbol_type()
		{
			return std::is_integral<A>::value || std::is_enum<S>::value;
		}

	public:
		typedef A AlphabetType;
		typedef S StateType;
		typedef std::function<void()> HandlerType;

		static_assert(check_state_type(), "State type should be integral or enum!");
		static_assert(check_symbol_type(), "Symbol type should be integral or enum!");

		struct Transition {
			HandlerType handler;
			StateType target;
		};

		template <typename Func>
		void addTransition(AlphabetType symbol, StateType from, StateType to, Func handler)
		{
			Transition t { handler, to };

			this->stt_[from][symbol] = std::move(t);
		}

		void raise(AlphabetType symbol)
		{
			Transition& ref = this->stt_[this->current_][symbol];

			ref.handler();
			this->current_ = ref.target;
		}

		void setTimeout(time_t tmo, AlphabetType symbol)
		{
			this->timer_value_ = tmo;
			this->timer_state_ = true;
			this->tmo_symbol_ = symbol;
		}

		void disableTimer()
		{
			this->timer_state_ = false;
		}

		void loop()
		{
			if(millis() >= this->timer_value_ && this->timer_state_) {
				this->timer_state_ = false;
				this->raise(this->tmo_symbol_);
			}
		}

	private:
		std::array<std::array<Transition, A_num>, S_num> stt_;
		StateType current_;
		time_t timer_value_;
		bool timer_state_;
		AlphabetType tmo_symbol_;
	};
}
