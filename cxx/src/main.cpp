#include <iostream>

#include "utils/async_timer.hpp"

int main(int argc, char* argv[])
{
	{
		zhf::async_timer timer;
		timer.post<1000>([] {
			std::cout << "hoho" << std::endl;
		});

		timer.post<1000>([] {
			std::cout << "coco" << std::endl;
		}, true);
		
		{
			auto counter_ptr = std::make_shared<int>(0);
			std::function<void()> do_send = [=, &timer] {
				std::cout << "hoho" << std::endl;
				
				(*counter_ptr)++;

				switch (*counter_ptr)
				{
				case 1:
					timer.post<1000>(do_send);
					break;
				case 2:
					timer.post<2000>(do_send);
					break;
				case 3:
					timer.post<4000>(do_send);
					break;
				default:
					break;
				}
			};
		}
		
		std::this_thread::sleep_for(std::chrono::seconds(20));
	}

	return 0;
}