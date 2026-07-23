#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/Yaml.h"
#include "/home/codeleaded/System/Static/Library/Geometry3D.h"

#include "/home/codeleaded/System/Static/Library/Lib3D_Cube.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mathlib.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_MeshFast.h"

typedef struct MCube3 {
    Cube3 cube;
    Vec3 o;
    Vec3 v;
    float l;
} MCube3;

World3D world;
Vector cubes;

Camera cam;
float vel = 0.0f;
float speed = 2.0f;
char jump = 0;
int Menu = 0;

void ReloadCubes(){
    Yaml yl = Yaml_Make("./data/World.yaml");
    
    if(yl.t.Root){
        Vector_Clear(&cubes);
        Branch* cubes_b = Yaml_GetBranch(&yl,"cubes");
        
        for(int i = 0;i<cubes_b->Childs.size;i++){
            Branch* c_b = *(Branch**)Vector_Get(&cubes_b->Childs,i);
            const Double x = Yaml_Branch_GetDouble(&yl,c_b,"x");
            const Double y = Yaml_Branch_GetDouble(&yl,c_b,"y");
            const Double z = Yaml_Branch_GetDouble(&yl,c_b,"z");

            const Double w = Yaml_Branch_GetDouble(&yl,c_b,"w");
            const Double h = Yaml_Branch_GetDouble(&yl,c_b,"h");
            const Double d = Yaml_Branch_GetDouble(&yl,c_b,"d");

            const Double l = Yaml_Branch_GetDouble(&yl,c_b,"l");

            const Double vx = Yaml_Branch_GetDouble(&yl,c_b,"vx");
            const Double vy = Yaml_Branch_GetDouble(&yl,c_b,"vy");
            const Double vz = Yaml_Branch_GetDouble(&yl,c_b,"vz");

            Vector_Push(&cubes,(MCube3[]){{
                .cube = {
                    {
                        .x = x,
                        .y = y,
                        .z = z
                    },
                    {
                        .x = w,
                        .y = h,
                        .z = d
                    }
                },
                .o = {
                    .x = x,
                    .y = y,
                    .z = z
                },
                .v = {
                    .x = vx,
                    .y = vy,
                    .z = vz
                },
                .l = l
            }});
        }
    }
    Yaml_Free(&yl);
}
void ReloadMesh(){
    Vector_Clear(&world.trisIn);

    for(int i = 0;i<cubes.size;i++){
        MCube3* c = (MCube3*)Vector_Get(&cubes,i);
		Lib3D_Cube(
            &world.trisIn,
            (Vec3D){ c->cube.p.x,c->cube.p.y,c->cube.p.z,1.0f },
            (Vec3D){ c->cube.d.x,c->cube.d.y,c->cube.d.z,1.0f },
            WHITE,
            WHITE
        );
	}

	Mesh_Shade(&world.trisIn,(Vec3D){ -0.5f,0.4f,-0.6f,1.0f });
}
void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window);
	}
	
	Menu = m;
}

void Setup(AlxWindow* w){
	Menu_Set(1);
    
    cam = Camera_Make(
		(Vec3D){ 0.5f,2.0f,0.5f,1.0f },
		(Vec3D){ 3.14 * 0.25f,0.0f,0.0f,1.0f },
		90.0f
	);

	world = World3D_Make(
		Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }),
		Matrix_MakePerspektive(cam.p,cam.up,cam.a),
		Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f)
	);
	world.normal = WORLD3D_NORMAL_CAP;
    cubes = Vector_New(sizeof(MCube3));

    ReloadCubes();
    ReloadMesh();
}
void Update(AlxWindow* w){
    if(Menu==1){
		Camera_Focus(&cam,GetMouseBefore(),GetMouse(),GetScreenRect().d);
		Camera_Update(&cam);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
    
    if(Stroke(ALX_KEY_ESC).PRESSED)
		Menu_Set(!Menu);

    if(Stroke(ALX_KEY_ENTER).PRESSED){
        ReloadCubes();
        ReloadMesh();
        cam.p = (Vec3D){ 0.5f,2.0f,0.5f,1.0f };
        cam.a = (Vec3D){ 0.0f,0.0f,0.0f,1.0f };
    }

    if(Stroke(ALX_KEY_W).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.fd,speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_S).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.fd,speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_A).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.sd,speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_D).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.sd,speed * w->ElapsedTime));
	
    if(Stroke(ALX_KEY_SPACE).DOWN && jump)
		vel = 2.0f;
	//if(Stroke(ALX_KEY_F).DOWN)
	//	vel -= 2.0f;

    vel += -5.0f * w->ElapsedTime;
    cam.p.y += vel * w->ElapsedTime;
    jump = 0;

    for(int i = 0;i<cubes.size;i++){
        MCube3* c = (MCube3*)Vector_Get(&cubes,i);
        
        const Vec3 dist = Vec3_Sub(c->cube.p,c->o);
        const float len = Vec3_Mag(dist);
        if(len >= c->l) c->v = Vec3_Neg(c->v);

        c->cube.p = Vec3_Add(c->cube.p,Vec3_Mulf(c->v,w->ElapsedTime));

        Cube3 pc = {
            .p = {
                .x = cam.p.x - 0.05f,
                .y = cam.p.y - 0.4f,
                .z = cam.p.z - 0.05f,
            },
            .d = {
                .x = 0.1f,
                .y = 0.5f,
                .z = 0.1f,
            }
        };

        const Side s = Resolve_Cube3_Cube3(&pc,c->cube);
        if(s == SIDE_TOP) jump = 1;
        if(s == SIDE_TOP || s == SIDE_BOTTOM) vel = 0.0f;

        cam.p.x = pc.p.x + 0.05f;
        cam.p.y = pc.p.y + 0.4f;
        cam.p.z = pc.p.z + 0.05f;

        if(s != SIDE_NONE){
            cam.p = Vec3D_Add(cam.p,Vec3D_Mul((Vec3D){ c->v.x,c->v.y,c->v.z },w->ElapsedTime));
        }
	}

	World3D_Set_Model(&world,Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }));
	World3D_Set_View(&world,Matrix_MakePerspektive(cam.p,cam.up,cam.a));
	World3D_Set_Proj(&world,Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f));

    ReloadMesh();

    Clear(LIGHT_BLUE);

	World3D_Update(&world,cam.p,(Vec2){ GetWidth(),GetHeight() });

	for(int i = 0;i<world.trisOut.size;i++){
		Tri3D* t = (Tri3D*)Vector_Get(&world.trisOut,i);
		const Pixel c = Pixel_Mulf(t->c.c,t->c.l);
		RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
	}
}
void Delete(AlxWindow* w){
    World3D_Free(&world);	
    Vector_Free(&cubes);
}

int main(){
    if(Create("Plattformer",1900,1000,1,1,Setup,Update,Delete))
        Start();
    return 0;
}