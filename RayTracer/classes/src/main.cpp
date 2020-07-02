/*
Name: Gareth Wiebe
Class: COMP 4490
Assignment 1ab
*/

#include <stdio.h>
#include <cmath>
#include <list>
#include <algorithm>
#include <limits>
#include <ctime>
#include <iostream>

#include "a1main.h"
#include "../COMP4490/sphere.h"
#include "../COMP4490/Vec3.h"
#include "../COMP4490/Color.h"
#include "../COMP4490/Ray.h"
#include "../COMP4490/Plane.h"
#include "../COMP4490/Triangle.h"
#include "../COMP4490/Box.h"
#include "../COMP4490/PointLight.h"
#include "../COMP4490/directionalLight.h"

using namespace std;

class Node;
SceneObject * findClosestObj(Ray ray, float &t);
Vec3 getRefractionVec(SceneObject * obj, Ray ray, float t);
Vec3 getReflectionVec(SceneObject * obj, Ray ray, float t);
Color calcLightingRecursive(Ray ray, int maxDepth);
SceneObject * findClosestObjBVH(Ray ray, float &t, list<SceneObject * >* list);
SceneObject* recurseBVH(Node * root, Ray & ray, float & t);
float scene1();
float scene2();
float sc3();
// reflection.ray
float sc4(Vec3 & cam);
// sphere_box.ray
float sc5(Vec3 & cam);
float sc6(Vec3 & cam);
Color focalLighting(Ray ray, int depth, int aperature, float focalDist);

Vec3 pointLight(250, 450, -180);
Sphere plHelp(pointLight, 5, 0.5);
Color ambC(249, 249, 122);// ambient
float ia = 0.5;
Color sky(59, 205, 221);
Vec3 camera;
std::list<Light * > lights;
std::list<SceneObject * > objs;
Node * BVHRoot;
bool useBVH;
int wWidth, wHeight;

int gX, gY;

class Node
{
public:
	Node(Box * aabb, std::list<SceneObject*>* list)
	{
		volume = aabb;
		subObjs = list;
		left = NULL;
		right = NULL;
	}

	Box * volume;
	std::list<SceneObject * >* subObjs;
	Node* left;
	Node* right;
};

Node* buildBVH(Vec3 minBound, Vec3 maxBound, int count)
{
	int minBoxSize = 700;
	Node* node = NULL;
	float minF = 99999.0F;
	float maxF = -99999.0F;
	float minX = minF, minY = minF, minZ = minF;
	float maxX = maxF, maxY = maxF, maxZ = maxF;

	list<SceneObject*> * localObjs = new list<SceneObject*>;

	// for objects
	for (auto const& o : objs) {

		Vec3 thisMin(0.0, 0.0, 0.0);
		Vec3 thisMax(0.0, 0.0, 0.0);

		bool isPlane = false;
		o->getMinMax(thisMin, thisMax);
		if (Plane * v = dynamic_cast<Plane*>(o))
			isPlane = true;

		//if (obj is not commpletly outside)
		if (!isPlane && !(thisMin.getX() > minBound.getX() && thisMax.getX() < maxBound.getX() &&
			thisMin.getY() > minBound.getY() && thisMax.getY() < maxBound.getY() &&
			thisMin.getZ() > minBound.getZ() && thisMax.getZ() < maxBound.getZ()))
		{
			// update min/max
			if (thisMin.getX() < minX) minX = thisMin.getX();
			if (thisMax.getX() > maxX) maxX = thisMax.getX();

			if (thisMin.getY() < minY) minY = thisMin.getY();
			if (thisMax.getY() > maxY) maxY = thisMax.getY();

			if (thisMin.getZ() < minZ) minZ = thisMin.getZ();
			if (thisMax.getZ() > maxZ) maxZ = thisMax.getZ();


			localObjs->push_back(o);
		} // else skip
	}

	if (localObjs->size() > 2 && (minBound.getX() == minF || ((abs(maxBound.getX() - minBound.getX()) > minBoxSize && maxBound.getX() > minBound.getX()) &&
		(abs(maxBound.getY() - minBound.getY()) > minBoxSize && maxBound.getY() > minBound.getY()) &&
		(abs(maxBound.getZ() - minBound.getZ()) > minBoxSize && maxBound.getZ() > minBound.getZ()))))
	{
		Box * aabb = NULL;
		// volume = (min xyz, max xyz)
		if (minBound.getX() == minF)
			aabb = new Box(Vec3(minX, minY, minZ), Vec3(maxX, maxY, maxZ), 0.5);
		else
			aabb = new Box(Vec3(minBound.getX(), minBound.getY(), minBound.getZ()), Vec3(maxBound.getX(), maxBound.getY(), maxBound.getZ()), 0.5);

		// newNode(volume, objlist = null)
		node = new Node(aabb, NULL);

		if (minBound.getX() == minF)
		{
			// nodeLeft = getVolume(minexyz, maxx/2yz)
			node->left = buildBVH(Vec3(minX, minY, minZ), Vec3(minX + (maxX - minX) / 2, maxY, maxZ), count);
			// nodeRight = getVolume(minex/2yz, maxxyz)
			node->right = buildBVH(Vec3(minX + (maxX - minX) / 2, minY, minZ), Vec3(maxX, maxY, maxZ), count);
		}
		else
		{
			float nextMinX = max(minX, minBound.getX()), nextMinY = max(minY, minBound.getY()), nextMinZ = max(minZ, minBound.getZ());
			float nextMaxX = min(maxX, maxBound.getX()), nextMaxY = min(maxY, maxBound.getY()), nextMaxZ = min(maxZ, maxBound.getZ());

			count++;
			count = count % 3;
			if (count == 0)
			{
				// nodeLeft = getVolume(minexyz, maxx/2yz)
				node->left = buildBVH(Vec3(nextMinX, nextMinY, nextMinZ), Vec3(nextMinX + (nextMaxX - nextMinX) / 2, nextMaxY, nextMaxZ), count);
				// nodeRight = getVolume(minex/2yz, maxxyz)
				node->right = buildBVH(Vec3(nextMinX + (nextMaxX - nextMinX) / 2, nextMinY, nextMinZ), Vec3(nextMaxX, nextMaxY, nextMaxZ), count);
			}
			if (count == 1)
			{
				// nodeLeft = getVolume(minexyz, maxx/2yz)
				node->left = buildBVH(Vec3(nextMinX, nextMinY, nextMinZ), Vec3(nextMinX, nextMinY + (nextMaxY - nextMinY) / 2, nextMaxZ), count);
				// nodeRight = getVolume(minex/2yz, maxxyz)
				node->right = buildBVH(Vec3(nextMinX, nextMinY + (nextMaxY - nextMinY) / 2, nextMinZ), Vec3(nextMaxX, nextMaxY, nextMaxZ), count);
			}
			if (count == 2)
			{
				// nodeLeft = getVolume(minexyz, maxx/2yz)
				node->left = buildBVH(Vec3(nextMinX, nextMinY, nextMinZ), Vec3(nextMinX, nextMaxY, nextMinZ + (nextMaxZ - nextMinZ) / 2), count);
				// nodeRight = getVolume(minex/2yz, maxxyz)
				node->right = buildBVH(Vec3(nextMinX, nextMinY, nextMinZ + (nextMaxZ - nextMinZ) / 2), Vec3(nextMaxX, nextMaxY, nextMaxZ), count);
			}

		}
	}
	else
	{
		//newNode(volume = null, objList = objsFound
		node = new Node(NULL, localObjs);
	}

	//objs.push_back(new Box(Vec3(minX, minY, minZ), Vec3(maxX, maxY, maxZ), 1));
	return node;
}

void trace(char *input_file, void *window, int width, int height) {
	Vec3 camera(width / 2.0f, height / 2.0f, -300.0f);
	int item = buildScene(input_file);

	wWidth = width;
	wHeight = height;

	float scale = 0;
	int apt = 1;
	float focus = 30;

	if (item == 1) scale = scene1();
	else if (item == 2)
	{
		scale = scene2();

		cout << "depth of field: aperture size is " << pow((apt * 2 + 1), 2) << ", focus is " << focus << endl;
	}
	else if (item == 3) scale = sc3();
	else if (item == 4) scale = sc4(camera);
	else if (item == 5) scale = sc5(camera);
	else if (item == 6) scale = sc6(camera);

	float minF = 99999.0F;
	float maxF = -99999.0F;

	BVHRoot = buildBVH(Vec3(minF, minF, minF), Vec3(maxF, maxF, maxF), 0);

	puts("Starting timer...");
	clock_t start = clock();

	// send ray through each pixel
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			gX = x; gY = y;
			Vec3 px(x / scale, y / scale, 0);
			Vec3 rayD = px - camera / scale;

			Ray ray = Ray(Vec3(px.getX(), px.getY(), px.getZ()), rayD.normalize());

			Color frag(0, 0, 0);
			if (item != 2) frag = calcLightingRecursive(ray, 5);
			else
			{
				frag = focalLighting(ray, 6, 1, 30);
			}


			set(window, x, y, frag.getR(), frag.getG(), frag.getB());
		}
	double duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	cout << "Time: " << duration << " seconds" << endl;
	cout << "Done" << endl;
}

void pick(void *window, int x, int y) {
	unsigned char r, g, b;
	if (get(window, x, y, r, g, b)) {
		printf("pick @ (%d, %d): colour [%d %d %d]\n", x, y, r, g, b);
		set(window, x, y, (128 + r) % 256, (128 + g) % 256, (128 + b) % 256);
		redraw(window);
	}
}

Color focalLighting(Ray ray, int depth, int aperature, float focalDist)
{
	if (aperature == 0) return calcLightingRecursive(ray, depth);
	// create focal distance point
	Vec3 focalP = ray.getOrigin() - camera;
	focalP = focalP + ray.getDirec() * focalDist;

	Color fragOut(0, 0, 0);
	float frac = 1;

	for (int aptX = -1 * aperature; aptX <= aperature; aptX++)
	{
		for (int aptY = -1 * aperature; aptY <= aperature; aptY++)
		{
			Vec3 newOrig(ray.getOrigin().getX() + aptX, ray.getOrigin().getY() + aptY, ray.getOrigin().getZ());
			Ray curRay(newOrig, (focalP - newOrig).normalize());
			Color clr1 = calcLightingRecursive(curRay, depth);

			frac++;

			if (frac != 1)
				fragOut = fragOut.blend(clr1, 1 / frac);
			else
				fragOut = clr1;
		}
	}

	return fragOut;
}

Color calcLighting(SceneObject * obj, Ray ray, float t)
{
	Color fragColor;

	// ambient
	float ka = obj->getKa();// ambient reflection const
	fragColor = fragColor + (obj->getColor().average(ambC))*ka*ia;

	if (!lights.empty())
	{
		float kd = obj->getKd();// diffuse reflection const
		float ks = obj->getKs();// specular reflection const
		int alpha = obj->getAlpha();

		// specular and diffuse
		for (auto const& i : lights) {
			Vec3 ptInterX = ray.getOrigin() + (ray.getDirec() * t);
			float dist = ptInterX.magnatude();

			Light * light = i;
			Color is = light->getIs();

			Vec3 lVec = light->getVec(ptInterX);
			float cAttenuation = 0;
			// attenuationK = 0.0035;
			float lAttenuation = 0;
			float qAttenuation = 0.0035;

			float falloff = 1;

			if (PointLight * v = dynamic_cast<PointLight*>(i))
			{
				cAttenuation = v->getKc();
				lAttenuation = v->getKl();
				qAttenuation = v->getKq();
				falloff = cAttenuation + lAttenuation * lVec.magnatude() + pow(lVec.magnatude()*qAttenuation, 2);
			}

			Vec3 objNorm = obj->getNormal(ptInterX);

			objNorm = objNorm.normalize();
			lVec = lVec.normalize();

			float dt = objNorm.dot(lVec);

			// check for shadows
			float t2 = 20000;
			bool clear = true;
			Ray ray2(ptInterX, lVec);
			Vec3 doO(0, 0, 0);

			for (auto const& o : objs) {
				doO = o->getPos() - obj->getPos();

				// if (doO.dot(lVec) > 0 && o != objs.back() && obj != o && o->intersect(ray2, t2) && t2>0) {
				if (doO.dot(lVec) > 0 && obj != o && o->intersect(ray2, t2) && t2 > 0) {
					clear = false;
					break;
				}
			}


			if (clear) {
				// diffuse
				fragColor = fragColor + obj->getColor() * (dt*kd) / falloff;

				// specular
				Vec3 h = lVec + ray.getDirec() * -1;
				h = h.normalize();
				Color spec(0, 0, 0);
				float nh = objNorm.dot(h);
				if (nh > 0) spec = is * pow(nh, alpha) * ks / falloff;

				fragColor = fragColor + spec;
			}

		}
	}

	return fragColor;
}

Color calcLightingRecursive(Ray ray, int maxDepth)
{
	Color fragColor;

	float t = 20000;

	SceneObject * obj = NULL;
	if (!useBVH) obj = findClosestObj(ray, t);
	else obj = recurseBVH(BVHRoot, ray, t);


	if (obj != NULL)
	{

		if (obj->isGlass() && maxDepth > 1)
		{
			Vec3 refractedVec = getRefractionVec(obj, ray, t);
			refractedVec = refractedVec.normalize();

			//t = abs(t);
			Vec3 ptInterX = ray.getOrigin() + (ray.getDirec() * t);

			Ray refractedRay(ptInterX, refractedVec);

			fragColor = calcLightingRecursive(refractedRay, maxDepth - 1);
			fragColor = fragColor.blend(calcLighting(obj, ray, t), 0.75);
		}
		else if (obj->isMirror() && maxDepth > 1)
		{
			Vec3 reflectedVec = getReflectionVec(obj, ray, t);
			reflectedVec = reflectedVec.normalize();

			//t = abs(t);
			Vec3 ptInterX = ray.getOrigin() + (ray.getDirec() * t);

			Ray reflectedRay(ptInterX, reflectedVec);

			fragColor = calcLightingRecursive(reflectedRay, maxDepth - 1);
			fragColor = fragColor.blend(calcLighting(obj, ray, t), 0.75);
		}
		else if (maxDepth > 1 && obj->getTrans() > 0)
		{
			fragColor = calcLighting(obj, ray, t);
			fragColor = fragColor.blend(calcLightingRecursive(Ray(ray.getOrigin() + ray.getDirec().normalize()*t*1.01, ray.getDirec()), maxDepth - 1), 1 - obj->getTrans());
		}
		else fragColor = calcLighting(obj, ray, t);
	}
	else  fragColor = Color(sky.getR(), sky.getG(), sky.getB());

	return fragColor;
}

Vec3 getReflectionVec(SceneObject * obj, Ray ray, float t)
{
	Vec3 l = ray.getDirec();
	l = l.normalize();

	Vec3 ptInterX = ray.getOrigin() + (ray.getDirec() * t);

	Vec3 n = obj->getNormal(ptInterX);
	n = n.normalize();

	float c = (n*-1).dot(l);

	return l + (n * c * 2);//reflected vector
}

Vec3 getRefractionVec(SceneObject * obj, Ray ray, float t)
{
	// refraction
	float n1 = 1;
	float n2 = 1.5;
	float r = n1 / n2;

	Vec3 l = ray.getDirec();
	l = l.normalize();

	Vec3 ptInterX = ray.getOrigin() + (ray.getDirec() * t);

	Vec3 n = obj->getNormal(ptInterX);
	n = n.normalize();
	float c = (n).dot(l);

	if (c < 0) // moving into glass
	{
		//n = n * -1;
		c = c * -1;
	}
	else // moving out of glass
	{
		n = n * -1;
		r = n2 / n1;
		//c = c * -1;
	}

	//Vec3 reflectV = l + (n * c * 2); // normalized
	//Vec3 refractV = l * r + n * (r*c - sqrt(1 - r * r * (1 - c * c))); // normalized
	float sinSq = 1 - r * r * (1 - c * c);
	if (sinSq < 0) return l + (n * c * 2);//reflected vector
	float sine = sqrt(sinSq);
	// num2 is cosinetheta 2
	//if (1 - r * r * num3 < 0) l = l * -1;
	//float num2 = sqrt(1 - r * r * num3);
	float num = (r*c - sine);
	Vec3 refractV = l * r + n * num; // normalized
									 //if (1 - r * r * num3 < 0) refractV = Vec3(refractV.getX()*-1, refractV.getY()*-1, refractV.getZ()*-1);

	return refractV;
}


SceneObject* recurseBVH(Node * root, Ray & ray, float & t)
{
	SceneObject * output = NULL;

	float tTemp, tTemp2;

	/*
	// recursive case
	if nodes has aabb & it intersects
	return recurse(left node)
	return recurse(right node)
	*/
	if (root->volume != NULL && root->volume->intersect(ray, tTemp))
	{
		SceneObject * left = NULL;
		SceneObject * right = NULL;

		if (root->left != NULL)
			left = recurseBVH(root->left, ray, tTemp);
		if (root->right != NULL)
			right = recurseBVH(root->right, ray, tTemp2);

		if (left != NULL && right != NULL && tTemp <= tTemp2)
		{
			t = tTemp;
			return left;
		}
		else if (left != NULL && right != NULL && tTemp2 <= tTemp)
		{
			t = tTemp2;
			return right;
		}
		else if (left != NULL)
		{
			t = tTemp;
			return left;
		}
		else if (right != NULL)
		{
			t = tTemp2;
			return right;
		}

	}
	// base case
	else if (root->volume)
	{
		return NULL;
	}
	else // leaf node
	{
		return findClosestObjBVH(ray, t, root->subObjs);
	}

	return output;
}

SceneObject * findClosestObjBVH(Ray ray, float &t, list<SceneObject * >* list)
{
	SceneObject * out = NULL;
	float cT = 30000;
	for (auto const& o : *list) {
		if (o->intersect(ray, t))
		{
			if (t < cT && t > 0 && (t > 0.1 || t < 0.1)) // t>1 so ray does not intersect with its own origin
			{
				out = o;
				cT = t;
			}
		}
	}
	t = cT;
	return out;
}

SceneObject * findClosestObj(Ray ray, float &t)
{
	SceneObject * out = NULL;
	float cT = 30000;
	for (auto const& o : objs) {
		if (o->intersect(ray, t))
		{
			if (t < cT && t > 0 && (t > 1 || t < 1)) // t>1 so ray does not intersect with its own origin
			{
				out = o;
				cT = t;
			}
		}
	}
	t = cT;
	return out;
}

float scene1()
{
	int width = wWidth;
	int height = wHeight;

	PointLight * pl = new PointLight(Vec3(240, 300, -150), Color(200, 200, 200));
	lights.push_back(pl);

	PointLight * pl2 = new PointLight(Vec3(width / 2.0f + 50, height / 2.0f + 350, 200.0f), Color(200, 200, 200));
	lights.push_back(pl2);

	directionalLight * dl = new directionalLight(Vec3(-200, -200, 70), Color(30, 20, 20));
	lights.push_back(dl);

	Sphere * sphere = new Sphere(Vec3(width / 2.0f, height / 2.0f, 150.0f), 100, 0.5);
	sphere->setGlass(true);
	objs.push_back(sphere);

	Sphere * sphere2 = new Sphere(Vec3(width / 2.0f + 220, height / 2.0f + 30, 130.0f), 100, 0.5);
	sphere2->setMirror(true);
	objs.push_back(sphere2);

	Sphere * sphere3 = new Sphere(Vec3(width / 2.0f - 520, height / 2.0f + 130, 1500.0f), 200, 0.5);
	objs.push_back(sphere3);

	Sphere * sphere4 = new Sphere(Vec3(width / 2.0f, 30, 150.0f), 80, 0.5);
	sphere4->setMirror(true);
	objs.push_back(sphere4);

	Triangle triangle(Vec3(100, 250, 500), Vec3(200, 250, 500), Vec3(100, 400, 500));
	Box * box = new Box(Vec3(width / 2 + 20, height / 2 - 50.0f, 390.0f), Vec3((width / 2.0f) + 300, (height / 2.0f) + 220, 450.0f), 0.5);
	objs.push_back(box);

	return 1;
}
float scene2()
{
	scene1();

	Plane * floor = new Plane(Vec3(0, 1, 0), Vec3(250, 0, 250));
	objs.push_back(floor);

	return 1;
}

float sc3()
{
	directionalLight * dl = new directionalLight(Vec3(200, 200, 70), Color(30, 20, 20));
	lights.push_back(dl);

	PointLight * pl = new PointLight(Vec3(240, 300, -150), Color(200, 200, 200));
	lights.push_back(pl);

	Sphere * sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 250.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 350.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 450.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 550.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 400, 30, 650.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);

	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 650.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 550.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 450.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 350.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 250.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f - 200, 40, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);

	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 650.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 550.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 450.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 350.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 250.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f, 40, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);

	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 650.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 550.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 450.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 350.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 250.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 200, 40, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);

	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 650.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 550.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 450.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 350.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 250.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);
	sphere1 = new Sphere(Vec3(wWidth / 2.0f + 400, 40, 150.0f), 40, 0.5);
	sphere1->setMirror(true);
	objs.push_back(sphere1);

	return 1;
}

// reflection.ray
float sc4(Vec3 & cam)
{
	cam.setZ(cam.getZ() * -1);

	ambC = Color(0, 0, 0);// ambient

	directionalLight * dl = new directionalLight(Vec3(0.169031, 0.565714, -8.07967), Color(255, 255, 255));
	lights.push_back(dl);

	PointLight * pl = new PointLight(Vec3(0.304256, 4.16571, -5.71324), Color(255, 255, 255 * 0.7), 0.25, 0.00337241, 4.5492e-05);
	lights.push_back(pl);

	pl = new PointLight(Vec3(-4.90189, 0.108571, -5.37518), Color(0, 255, 0), 0.25, 0.00337241, 4.5492e-05);
	lights.push_back(pl);

	Sphere * sphere = new Sphere(Vec3(0.507093f, 0.337143f, -6.72743f), 0.593599f, Color(0, 255 * 0.4, 255 * 0.4), 0.4, 0.8, 0.4, 102);
	sphere->setMirror(true);
	objs.push_back(sphere);

	sphere = new Sphere(Vec3(-1.85934f, 0.188571f, -5.84847f), 0.791465f, Color(0, 255 * 0.4, 255 * 0.4), 0.4f, 0.8f, 0.4f, 115);
	sphere->setMirror(true);
	objs.push_back(sphere);

	return 160;
}

// sphere_box.ray
float sc5(Vec3 & cam)
{
	cam.setZ(cam.getZ() * -1);

	ambC = Color(0, 0, 0);// ambient

	directionalLight * dl = new directionalLight(Vec3(0, 0, -3), Color(255, 255, 255));
	lights.push_back(dl);

	float xScale = 0.8, yScale = 0.8, zScale = 4;
	float x = -0.2, y = -0.5, z = -3.5;
	float minX = x - xScale / 2, minY = y - yScale / 2, minZ = z - zScale / 2;
	float maxX = x + xScale / 2, maxY = y + yScale / 2, maxZ = z + zScale / 2;
	Box * box = new Box(Vec3(minX, minY, minZ), Vec3(maxX, maxY, maxZ), 0.5);
	box->setTrans(0.5);
	objs.push_back(box);

	Sphere * sphere = new Sphere(Vec3(0, 0, -2.50), 0.70, Color(255 * 0.5, 255 * 0.5, 255 * 0.9), 0.4, 0.8, 0.5, 70);
	sphere->setTrans(0.9);
	objs.push_back(sphere);

	return 160;
}

// trimesh demonstration
float sc6(Vec3 & cam)
{
	cam.setZ(cam.getZ() * -1);
	directionalLight * dl = new directionalLight(Vec3(200, 200, 70), Color(30, 20, 20));
	lights.push_back(dl);

	dl = new directionalLight(Vec3(0, 0, -3), Color(255, 255, 255));
	lights.push_back(dl);

	PointLight * pl = new PointLight(Vec3(240, 300, -150), Color(200, 200, 200));
	lights.push_back(pl);

	pl = new PointLight(Vec3(0.304256, 4.16571, -5.71324), Color(255, 255, 255 * 0.7), 0.25, 0.00337241, 4.5492e-05);
	lights.push_back(pl);

	Vec3 * meshPoints[] = {
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.643832,0.0386265,-3.12428),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.559869,0.046692,-3.12974),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.529432,0.0750963,-3.2033),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.570356,0.107199,-3.30188),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.658664,0.124201,-3.36772),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.742628,0.116133,-3.36225),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.773063,0.0877272,-3.28869),
		new Vec3(0.73214,0.0556245,-3.19012),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.636596,-0.0369209,-3.09606),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.518846,-0.0256099,-3.10372),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.476162,0.0142244,-3.20689),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.533553,0.0592468,-3.34513),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.657395,0.0830855,-3.43746),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.775146,0.0717749,-3.4298),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.817827,0.0319399,-3.32663),
		new Vec3(0.760438,-0.0130827,-3.18839),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.636195,-0.053747,-3.12837),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.534582,-0.043986,-3.13498),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.497748,-0.00961244,-3.22401),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.547274,0.0292422,-3.3433),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.654144,0.0498121,-3.42298),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.755758,0.0400542,-3.41638),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.79259,0.00567849,-3.32735),
		new Vec3(0.743065,-0.0331771,-3.20805),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.769663,0.412501,-3.25606),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769731,0.412445,-3.25599),
		new Vec3(0.769745,0.412402,-3.2559),
		new Vec3(0.769731,0.412445,-3.25599),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769745,0.412402,-3.2559),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769708,0.412389,-3.25584),
		new Vec3(0.769619,0.412397,-3.25578),
		new Vec3(0.769708,0.412389,-3.25584),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769619,0.412397,-3.25578),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.768028,0.413012,-3.256),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.768028,0.413012,-3.256),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.768455,0.412927,-3.25611),
		new Vec3(0.768817,0.412837,-3.25617),
		new Vec3(0.768455,0.412927,-3.25611),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.768817,0.412837,-3.25617),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769118,0.412746,-3.25618),
		new Vec3(0.769358,0.412656,-3.25616),
		new Vec3(0.769118,0.412746,-3.25618),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.769619,0.412397,-3.25578),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.757264,0.406641,-3.22537),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.757264,0.406641,-3.22537),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.768028,0.413012,-3.256),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.757264,0.406641,-3.22537),
		new Vec3(0.767536,0.413091,-3.25584),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.768028,0.413012,-3.256),
		new Vec3(0.768455,0.412927,-3.25611),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.768028,0.413012,-3.256),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.768455,0.412927,-3.25611),
		new Vec3(0.768817,0.412837,-3.25617),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.768455,0.412927,-3.25611),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.768817,0.412837,-3.25617),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.769118,0.412746,-3.25618),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.768817,0.412837,-3.25617),
		new Vec3(0.769118,0.412746,-3.25618),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.769118,0.412746,-3.25618),
		new Vec3(0.769358,0.412656,-3.25616),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.769358,0.412656,-3.25616),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.769663,0.412501,-3.25606),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.769539,0.412573,-3.25612),
		new Vec3(0.769663,0.412501,-3.25606),
		new Vec3(0.765952,0.401204,-3.25855),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.769663,0.412501,-3.25606),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.765952,0.401204,-3.25855),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.765952,0.401204,-3.25855),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.769663,0.412501,-3.25606),
		new Vec3(0.769731,0.412445,-3.25599),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.769731,0.412445,-3.25599),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.769731,0.412445,-3.25599),
		new Vec3(0.769745,0.412402,-3.2559),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.769708,0.412389,-3.25584),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.769745,0.412402,-3.2559),
		new Vec3(0.769708,0.412389,-3.25584),
		new Vec3(0.768736,0.400585,-3.25211),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.768736,0.400585,-3.25211),
		new Vec3(0.769619,0.412397,-3.25578),
		new Vec3(0.768736,0.400585,-3.25211),
		new Vec3(0.769708,0.412389,-3.25584),
		new Vec3(0.769619,0.412397,-3.25578),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.768736,0.400585,-3.25211),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.768736,0.400585,-3.25211),
		new Vec3(0.76831,0.400971,-3.2486),
		new Vec3(0.757264,0.406641,-3.22537),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.759493,0.327672,-3.21574),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.759493,0.327672,-3.21574),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.763856,0.360553,-3.23324),
		new Vec3(0.759493,0.327672,-3.21574),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.752176,0.309598,-3.27653),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.752176,0.309598,-3.27653),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.757264,0.406641,-3.22537),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.717668,0.349385,-3.1479),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.717668,0.349385,-3.1479),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.717668,0.349385,-3.1479),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.734701,0.413193,-3.21041),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.717668,0.349385,-3.1479),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.717668,0.349385,-3.1479),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.732469,0.411145,-3.23077),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.751722,0.405559,-3.25317),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.556531,0.325771,-3.04348),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.556531,0.325771,-3.04348),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.758066,0.403686,-3.25762),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.761183,0.402716,-3.25885),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.669106,0.37214,-3.20841),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.765952,0.401204,-3.25855),
		new Vec3(0.763827,0.401875,-3.25913),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.705776,0.37419,-3.24587),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.572926,0.295222,-3.15177),
		new Vec3(0.76751,0.400741,-3.25715),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.759644,0.368307,-3.26322),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.768455,0.400524,-3.25499),
		new Vec3(0.759644,0.368307,-3.26322),
		new Vec3(0.746899,0.354132,-3.19187),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.759493,0.327672,-3.21574),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.759493,0.327672,-3.21574),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.68109,0.349096,-3.1177),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.638622,0.358619,-3.126),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.556531,0.325771,-3.04348),
		new Vec3(0.640783,0.367641,-3.1669),
		new Vec3(0.732192,0.373522,-3.26376),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.643676,0.346861,-3.21875),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.614777,0.315563,-3.20188),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.759644,0.368307,-3.26322),
		new Vec3(0.748364,0.371417,-3.26758),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.691259,0.347864,-3.25925),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.664837,0.322666,-3.24941),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.661719,0.313256,-3.25075),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.646361,0.29334,-3.2449),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.646361,0.29334,-3.2449),
		new Vec3(0.650218,0.273181,-3.2483),
		new Vec3(0.646361,0.29334,-3.2449),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.650218,0.273181,-3.2483),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.650218,0.273181,-3.2483),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.759644,0.368307,-3.26322),
		new Vec3(0.765115,0.364562,-3.2515),
		new Vec3(0.73859,0.320557,-3.15903),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.756472,0.302809,-3.21963),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.702992,0.314747,-3.0955),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.699458,0.278429,-3.10053),
		new Vec3(0.660634,0.311729,-3.04232),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.607147,0.266095,-3.0116),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.607147,0.266095,-3.0116),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.556531,0.325771,-3.04348),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.605214,0.314333,-3.01354),
		new Vec3(0.542198,0.293909,-3.09091),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.556531,0.325771,-3.04348),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.595601,0.342683,-3.16297),
		new Vec3(0.560914,0.336392,-3.10541),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.542198,0.293909,-3.09091),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.542198,0.293909,-3.09091),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.73375,0.344099,-3.2752),
		new Vec3(0.758565,0.336522,-3.25949),
		new Vec3(0.735026,0.290634,-3.16369),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.737643,0.282986,-3.17503),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.607147,0.266095,-3.0116),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.657122,0.269188,-3.04562),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.607147,0.266095,-3.0116),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.607147,0.266095,-3.0116),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.542198,0.293909,-3.09091),
		new Vec3(0.552099,0.276389,-3.02807),
		new Vec3(0.542198,0.293909,-3.09091),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.569631,0.305354,-3.14516),
		new Vec3(0.572926,0.295222,-3.15177),
		new Vec3(0.752176,0.309598,-3.27653),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.715543,0.324621,-3.27941),
		new Vec3(0.753538,0.31732,-3.27121),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.704051,0.271092,-3.11677),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.734755,0.272178,-3.17303),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.737125,0.263288,-3.18412),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.74949,0.252214,-3.22368),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.745965,0.159598,-3.27768),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.745965,0.159598,-3.27768),
		new Vec3(0.746626,0.144926,-3.31211),
		new Vec3(0.745965,0.159598,-3.27768),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.746626,0.144926,-3.31211),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.74755,0.12492,-3.35949),
		new Vec3(0.746626,0.144926,-3.31211),
		new Vec3(0.74755,0.12492,-3.35949),
		new Vec3(0.748771,0.152934,-3.32613),
		new Vec3(0.66357,0.261999,-3.06598),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.610654,0.241794,-3.02317),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.610654,0.241794,-3.02317),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.615234,0.258644,-3.03386),
		new Vec3(0.531855,0.267892,-3.08351),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.561118,0.267558,-3.04605),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.531855,0.267892,-3.08351),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.572926,0.295222,-3.15177),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.548737,0.284056,-3.10214),
		new Vec3(0.572926,0.295222,-3.15177),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.572926,0.295222,-3.15177),
		new Vec3(0.614443,0.305598,-3.20492),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.752176,0.309598,-3.27653),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.710609,0.316265,-3.28158),
		new Vec3(0.752176,0.309598,-3.27653),
		new Vec3(0.757459,0.294854,-3.22678),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.755421,0.285674,-3.2264),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.756028,0.276878,-3.23299),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.749402,0.270072,-3.29732),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.749402,0.270072,-3.29732),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.749402,0.270072,-3.29732),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.7247,0.21356,-3.32119),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.7247,0.21356,-3.32119),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.700266,0.258208,-3.11252),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.610654,0.241794,-3.02317),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.658597,0.247026,-3.05875),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.610654,0.241794,-3.02317),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.610654,0.241794,-3.02317),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.531855,0.267892,-3.08351),
		new Vec3(0.551677,0.249805,-3.02919),
		new Vec3(0.531855,0.267892,-3.08351),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.531855,0.267892,-3.08351),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.554352,0.281108,-3.13598),
		new Vec3(0.595876,0.293069,-3.19086),
		new Vec3(0.645276,0.302622,-3.24058),
		new Vec3(0.646361,0.29334,-3.2449),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.650218,0.273181,-3.2483),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.646361,0.29334,-3.2449),
		new Vec3(0.693983,0.307965,-3.27636),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.747909,0.303195,-3.28149),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.745663,0.295441,-3.28699),
		new Vec3(0.749402,0.270072,-3.29732),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.7247,0.21356,-3.32119),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.749402,0.270072,-3.29732),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.705317,0.249143,-3.12895),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.666876,0.237683,-3.07973),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.624451,0.232046,-3.04704),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.57027,0.239321,-3.04997),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.548291,0.25725,-3.0975),
		new Vec3(0.566067,0.271391,-3.14776),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.601978,0.283493,-3.1983),
		new Vec3(0.650218,0.273181,-3.2483),
		new Vec3(0.689016,0.299054,-3.278),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.678643,0.276878,-3.28076),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.680854,0.143759,-3.31808),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.680854,0.143759,-3.31808),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.680854,0.143759,-3.31808),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.698619,0.243448,-3.15438),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.726694,0.246807,-3.1864),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.634677,0.0660677,-3.12504),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.527663,0.118692,-3.10744),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.666743,0.243459,-3.13444),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.527663,0.118692,-3.10744),
		new Vec3(0.630853,0.250194,-3.14089),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.617053,0.260522,-3.17592),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.627751,0.267511,-3.21139),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.630312,0.192927,-3.27409),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.710324,0.277324,-3.30217),
		new Vec3(0.7247,0.21356,-3.32119),
		new Vec3(0.761455,0.258625,-3.25804),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.637445,0.113404,-3.09627),
		new Vec3(0.634677,0.0660677,-3.12504),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.634677,0.0660677,-3.12504),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.684395,0.135391,-3.14608),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.72441,0.162466,-3.20723),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.575998,0.103209,-3.07295),
		new Vec3(0.527663,0.118692,-3.10744),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.577066,0.169832,-3.22215),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.527663,0.118692,-3.10744),
		new Vec3(0.537041,0.145249,-3.16697),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.703431,0.185544,-3.32934),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.703431,0.185544,-3.32934),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.660653,0.170804,-3.30861),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.748127,0.124726,-3.35936),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.703431,0.185544,-3.32934),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.7247,0.21356,-3.32119),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.68154,0.208809,-3.31),
		new Vec3(0.748745,0.205703,-3.30411),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.74832,0.18704,-3.26252),
		new Vec3(0.745965,0.159598,-3.27768),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.746626,0.144926,-3.31211),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.745965,0.159598,-3.27768),
		new Vec3(0.746626,0.144926,-3.31211),
		new Vec3(0.748771,0.152934,-3.32613),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.721622,0.128592,-3.22546),
		new Vec3(0.634677,0.0660677,-3.12504),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.681155,0.0938451,-3.16863),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.634677,0.0660677,-3.12504),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.623018,0.0982537,-3.24444),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.623018,0.0982537,-3.24444),
		new Vec3(0.74825,0.124784,-3.35925),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.623018,0.0982537,-3.24444),
		new Vec3(0.570343,0.0546987,-3.11322),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.680854,0.143759,-3.31808),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.623018,0.0982537,-3.24444),
		new Vec3(0.536981,0.0793681,-3.16175),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.680854,0.143759,-3.31808),
		new Vec3(0.560668,0.110791,-3.21472),
		new Vec3(0.607322,0.143094,-3.26619),
		new Vec3(0.732873,0.187474,-3.32902),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.703431,0.185544,-3.32934),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.703431,0.185544,-3.32934),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.748104,0.178491,-3.31108),
		new Vec3(0.748771,0.152934,-3.32613),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.74768,0.124826,-3.35948),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.748771,0.152934,-3.32613),
		new Vec3(0.74755,0.12492,-3.35949),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.748196,0.124778,-3.35923),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.726882,0.121945,-3.27328),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.748196,0.124778,-3.35923),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.74825,0.124784,-3.35925),
		new Vec3(0.748196,0.124778,-3.35923),
		new Vec3(0.694627,0.0980349,-3.23428),
		new Vec3(0.652201,0.0839415,-3.21453),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.748246,0.124758,-3.35929),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.748246,0.124758,-3.35929),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.623018,0.0982537,-3.24444),
		new Vec3(0.643286,0.120927,-3.28233),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.713326,0.157647,-3.33866),
		new Vec3(0.748127,0.124726,-3.35936),
		new Vec3(0.748127,0.124726,-3.35936),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.748035,0.124723,-3.3594),
		new Vec3(0.748035,0.124723,-3.3594),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.747928,0.124732,-3.35943),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.747928,0.124732,-3.35943),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.748035,0.124723,-3.3594),
		new Vec3(0.727717,0.160978,-3.34284),
		new Vec3(0.738778,0.161162,-3.34188),
		new Vec3(0.745978,0.158415,-3.33619),
		new Vec3(0.74768,0.124826,-3.35948),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.748771,0.152934,-3.32613),
		new Vec3(0.74755,0.12492,-3.35949),
		new Vec3(0.74768,0.124826,-3.35948),
		new Vec3(0.747928,0.124732,-3.35943),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.748035,0.124723,-3.3594),
		new Vec3(0.748127,0.124726,-3.35936),
		new Vec3(0.748035,0.124723,-3.3594),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.748127,0.124726,-3.35936),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.748246,0.124758,-3.35929),
		new Vec3(0.748199,0.124738,-3.35932),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.748246,0.124758,-3.35929),
		new Vec3(0.74825,0.124784,-3.35925),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.748196,0.124778,-3.35923),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.748196,0.124778,-3.35923),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.74755,0.12492,-3.35949),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.74768,0.124826,-3.35948),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.74768,0.124826,-3.35948),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.748102,0.124755,-3.35923),
		new Vec3(0.748265,0.124772,-3.35926),
		new Vec3(0.747808,0.124767,-3.35946),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.647601,0.0715689,-3.15614),
		new Vec3(0.707801,0.0831573,-3.20103),
		new Vec3(0.647601,0.0715689,-3.15614),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.707801,0.0831573,-3.20103),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.647601,0.0715689,-3.15614),
		new Vec3(0.707801,0.0831573,-3.20103),
		new Vec3(0.735698,0.105045,-3.26823),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.714951,0.124409,-3.31838),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.735698,0.105045,-3.26823),
		new Vec3(0.714951,0.124409,-3.31838),
		new Vec3(0.735698,0.105045,-3.26823),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.714951,0.124409,-3.31838),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.755717,0.141984,-3.36927),
		new Vec3(0.735698,0.105045,-3.26823),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.707801,0.0831573,-3.20103),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.735698,0.105045,-3.26823),
		new Vec3(0.707801,0.0831573,-3.20103),
		new Vec3(0.743908,0.0738452,-3.17543),
		new Vec3(0.789992,0.11,-3.28644),
		new Vec3(0.647601,0.0715689,-3.15614),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.590363,0.0770677,-3.15987),
		new Vec3(0.569616,0.0964309,-3.21001),
		new Vec3(0.590363,0.0770677,-3.15987),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.569616,0.0964309,-3.21001),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.590363,0.0770677,-3.15987),
		new Vec3(0.569616,0.0964309,-3.21001),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.549909,0.0637861,-3.10744),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.590363,0.0770677,-3.15987),
		new Vec3(0.647601,0.0715689,-3.15614),
		new Vec3(0.590363,0.0770677,-3.15987),
		new Vec3(0.644462,0.0547028,-3.10128),
		new Vec3(0.569616,0.0964309,-3.21001),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.597513,0.118318,-3.27722),
		new Vec3(0.657713,0.129907,-3.3221),
		new Vec3(0.597513,0.118318,-3.27722),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.657713,0.129907,-3.3221),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.597513,0.118318,-3.27722),
		new Vec3(0.657713,0.129907,-3.3221),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.652631,0.100382,-3.23925),
		new Vec3(0.714951,0.124409,-3.31838),
		new Vec3(0.657713,0.129907,-3.3221),
		new Vec3(0.661165,0.15107,-3.37543),
		new Vec3(0.657713,0.129907,-3.3221),
		new Vec3(0.714951,0.124409,-3.31838),
		new Vec3(0.561719,0.131927,-3.30129),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.597513,0.118318,-3.27722),
		new Vec3(0.569616,0.0964309,-3.21001),
		new Vec3(0.597513,0.118318,-3.27722),
		new Vec3(0.515634,0.0957719,-3.19028),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.699782,-0.0336575,-3.24222),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.727923,-0.0115806,-3.31),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.699782,-0.0336575,-3.24222),
		new Vec3(0.727923,-0.0115806,-3.31),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.751791,-0.0498736,-3.20618),
		new Vec3(0.727923,-0.0115806,-3.31),
		new Vec3(0.706994,0.00795491,-3.3606),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.806231,-0.00716455,-3.33731),
		new Vec3(0.706994,0.00795491,-3.3606),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.706994,0.00795491,-3.3606),
		new Vec3(0.649252,0.0135,-3.36435),
		new Vec3(0.765742,0.0306227,-3.43518),
		new Vec3(0.649252,0.0135,-3.36435),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.588522,0.00180881,-3.31907),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.649252,0.0135,-3.36435),
		new Vec3(0.588522,0.00180881,-3.31907),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.654044,0.0413557,-3.44245),
		new Vec3(0.588522,0.00180881,-3.31907),
		new Vec3(0.56038,-0.0202671,-3.25129),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.536565,0.0187428,-3.35486),
		new Vec3(0.56038,-0.0202671,-3.25129),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.56038,-0.0202671,-3.25129),
		new Vec3(0.58131,-0.0398004,-3.2007),
		new Vec3(0.482126,-0.0239685,-3.22372),
		new Vec3(0.58131,-0.0398004,-3.2007),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.639052,-0.0453477,-3.19694),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.58131,-0.0398004,-3.2007),
		new Vec3(0.639052,-0.0453477,-3.19694),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.522614,-0.0617557,-3.12586),
		new Vec3(0.639052,-0.0453477,-3.19694),
		new Vec3(0.699782,-0.0336575,-3.24222),
		new Vec3(0.634312,-0.0724851,-3.11859),
		new Vec3(0.639052,-0.0453477,-3.19694),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.699782,-0.0336575,-3.24222),
		new Vec3(0.727923,-0.0115806,-3.31),
		new Vec3(0.699782,-0.0336575,-3.24222),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.639052,-0.0453477,-3.19694),
		new Vec3(0.58131,-0.0398004,-3.2007),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.56038,-0.0202671,-3.25129),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.58131,-0.0398004,-3.2007),
		new Vec3(0.56038,-0.0202671,-3.25129),
		new Vec3(0.588522,0.00180881,-3.31907),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.588522,0.00180881,-3.31907),
		new Vec3(0.649252,0.0135,-3.36435),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.706994,0.00795491,-3.3606),
		new Vec3(0.644073,-0.016997,-3.28103),
		new Vec3(0.649252,0.0135,-3.36435),
		new Vec3(0.706994,0.00795491,-3.3606),
		new Vec3(0.727923,-0.0115806,-3.31),
		new Vec3(0.644073,-0.016997,-3.28103)
	};


	Triangle * tri = NULL;
	int meshSize = sizeof(meshPoints) / sizeof(meshPoints[0]);
	for (int i = 0; i < meshSize; i += 3)
	{
		tri = new Triangle(*meshPoints[i + 0]*1000, *meshPoints[i + 1]*1000, *meshPoints[i + 2]*1000);
		objs.push_back(tri);
	}

	cout << "Loaded " << meshSize << " polygons" << endl;

	return 160;
}

int buildScene(char * input_file)
{
	try
	{
		string str(input_file);
		if (str.length() > 2)
		{
			char input = input_file[0];
			char acl = input_file[2];
			int out = input - '0';

			if (out < 4 || out > 6)cout << "load custom scene " << out << endl;
			else if (out == 4) cout << "load reflection.ray" << endl;
			else if (out == 5) cout << "load sphere_box.ray" << endl;
			else if (out == 6) cout << "load trimesh1.ray" << endl;


			useBVH = acl == '1';
			if (useBVH)
			{
				cout << "Using acceleration" << endl;
			}
			else cout << "Not using acceleration" << endl;
			return out;
		}
		else
		{
			puts("Usage: comp4490.exe \"<Scene number> <Octree toggle>\"\n\t Example: comp4490.exe \"1 1\"");
			cin.get();
			exit(0);
		}
	}
	catch (...)
	{
		puts("Usage: comp4490.exe \"<Scene number> <Octree toggle>\"\n\t Example: comp4490.exe \"1 1\"");
		cin.get();
		exit(0);
	}
}