//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection pos = this->intersect(ray);
    if(!pos.happened)
        return {0.0f};
    return Shade(pos, -ray.direction, depth);
}
Vector3f Scene::Shade(const Intersection& hint, const Vector3f& wo, int depth) const{
    
    Vector3f L_dir(0.0f, 0.0f, 0.0f);
    if(hint.m->hasEmission()){
        return hint.m->getEmission();
    }
    if(depth > this->maxDepth)
    {
        return L_dir;
    }
    const float Epsinol = 0.0005f;
    float pdf_ligth;
    Intersection inter;
    sampleLight(inter, pdf_ligth);
    Vector3f& NN = inter.normal;
    Vector3f& emit = inter.emit;
    Vector3f ws = normalize(inter.coords - hint.coords);
    Ray PosToLight(hint.coords, ws); // p to x(light);
   
    
    Vector3f D = intersect(PosToLight).coords - inter.coords;
    if(D.norm() < Epsinol){
        float cosTheta = dotProduct(ws, hint.normal);
        float cosTheta_1 = dotProduct(-ws, NN);
        float distance = pow((inter.coords - hint.coords).norm(), 2);
        L_dir = inter.emit * hint.m->eval(wo, ws, hint.normal) * cosTheta * cosTheta_1 / distance / pdf_ligth;
    }
    Vector3f L_indir(0.0f, 0.0f, 0.0f);

    Vector3f wi = normalize(hint.m->sample(wo, hint.normal)); // wi
    Ray rayi(hint.coords, wi);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);

    // 生成0到1之间的随机数
    float randomNum = dis(gen);
    if(randomNum < 0.8){
        Intersection q = intersect(rayi);
        float pdf = hint.m->pdf(wo, wi, hint.normal);
        if(pdf > Epsinol){
            if(q.happened && !q.m->hasEmission()){
                float cosTheta = dotProduct(wi, hint.normal);
                Vector3f f_r = hint.m->eval(wo, wi, hint.normal);
                
                L_indir = Shade(q, wi, depth + 1) * f_r * cosTheta / pdf / RussianRoulette;
            }
        }

    }
    return L_dir + L_indir;

    //  if(hint.m->hasEmission()) {
    //     return hint.m->getEmission();
    // }

    // const float epsilon = 0.005f;

    // // Sample from Area Light
    // Vector3f L_dir = Vector3f(0.);  // Direct Light
    // Intersection lightSamplePos;
    // float lightPdf;
    // sampleLight(lightSamplePos, lightPdf);

    // Vector3f p = hint.coords, x = lightSamplePos.coords;
    // Vector3f ws = normalize(x - p);
    // Vector3f NN = lightSamplePos.normal, N = hint.normal;

    // if((intersect(Ray(p, ws)).coords - x).norm() < epsilon) {
    //     L_dir = lightSamplePos.emit * hint.m->eval(wo, ws, N)
    //     * dotProduct(N, ws) * dotProduct(NN, -ws) / ((x-p).norm() * (x-p).norm()) / lightPdf;
    // }

    // Vector3f L_indir;  // Indirect Light
    // if(get_random_float() < RussianRoulette) {
    //     Vector3f wi = hint.m->sample(wo, hint.normal).normalized();
    //     float pdf = hint.m->pdf(wo, wi, hint.normal);
    //     if(pdf > epsilon) {
    //         Intersection hit = intersect(Ray(hint.coords, wi));
    //         if(hit.happened && !hit.m->hasEmission()) {
    //             L_indir = Shade(hit, wi, depth + 1) * hint.m->eval(wo, wi, hint.normal) *
    //                     dotProduct(wi, hint.normal) / pdf / RussianRoulette;
    //         }
    //     }
    // }

    // return L_dir + L_indir;
}