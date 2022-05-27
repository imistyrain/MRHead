#include<iostream>
using std::cin;
using std::cout;

int main() {
	const int image_width = 256;
	const int image_height = 256;
	cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	for (int j = image_height - 1; j >= 0; --j) {
		for (int i = 0; i < image_width; ++i) {
			double r = double(i) / (image_width - 1);
			double g = double(j) / (image_height - 1);
			double b = 0.25;
			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);
			cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
}