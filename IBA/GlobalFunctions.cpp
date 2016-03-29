#include "StdAfx.h"

#include "GlobalFunctions.h"
#include "LocalCenter\G2XCommonTool.h"


CString IBAGlobal::MakeUserName()
{
	static const char* s_names[] = {"������","�����","Ѧ����","��׳��","Ǯ�ڈ�","�˶���","�¹���","κ����","��׿��","���ٰ�","��˳��","���˹","�����","�׽�ά","������","��չ��","������","ʱ����","�ܳ���","������","�Ż�ɭ","�ľ���","������","������","������","�ް���","��ʩ��","������","��ǧ��","������","������","�뺮�N","������","¬�վ�","��׳��","����׳","�ư���","������","���ݸ�","������","�츶��","½�Է�","��ة��","���ٻ�","�ﵾ��","Ī����","��Ԩ��","������","����","���ʷ�","�����","׿��֪","������","Ī����","����","ᯱ���","�ճ���","������","�����","�θ���","���ξ�","¬ȫ��","��ϯ��","��˳��","ŷ����","��ʼ��","�ź��","������","���»","�����","��ĽӪ","���ݿ","�����","����ұ","������","���ɹ�","��ϼ�","��ͼ��","�س���","�����","�����","Ԫ����","½ʾ��","��ʤ��","�ƽ���","��ҹ�","������","��Ȼ��","�־���","������","������","�վ���","���ϸ�","������","������","·����","����ͮ","�Ͼ��","������","������","��ք�","���","������","������","����","������","����","��ͤ��","³����","���N��","���ű�","����","��Ľ��","�ر�","������","�⴨��","�ѐ���","ŷ�ʻ�","����","��Ƶ��","�����","���ܵ�","����ά","�̱�ӳ","�ɺӲ�","�����","�ܹ���","������","�޶ȳ�","�ž�ͯ","��ӱ��","����Ը","֣ҹ��","�����","��Ԫ��","����ط","���¹�","���ų�","����ҫ","�����","��ԭ��","������","����","����ũ","Τ���","�ֱ���","ϯ��ϲ","��׼��","��Ʊ��","��׿��","��Լ��","�ĸ���","�����","����ӳ","�´�ӳ","Ϳ����","ΤƷӽ","����ޱ","������","������","������","�̱�Ⱥ","������","ʩ����","��ΰ��","��楱�","ʩ����","������","������","����ӭ","������","����","������","������","���ӱ�","�Գ�ӧ","Τ����","�Ŵ���","��ȵ��","����ԥ","������","Ī����","������","Τ��Ȼ","Ǯ��Ӣ","�ξ�","����Ѥ","Ԭ��Ⱥ","�Ѿ���","������","������","����Ը","������","���Ŀ�","����ˬ","������","��ӯ��","���氮","������","������","�����","�ɳξ�","�ֳ��","Ī����","������","��ة�","ϯ����","÷��֥","�˱���","����΢","������","������","������","�ְ��","����Ⱥ","�ϻ���","���Ĵ�","��ԣݼ","�׽�ޱ","��ͮ��","������","�����","������","�����","��ܰ��","³�б�","��Ľ��","������","����֥","�����","������","�����","Ԭ˳��","������","�ű���","����ԥ","������","������","Ԭ����","��ĺ��","������","������","�޳���","Ѧ��ѩ","���ٻ","������","������","�����","����ٻ","�ձ���","����Ⱥ","÷���","�����","����ӱ","������","���޾�","���ݷ�","Ϳ���","����ө","����֦","��ͼ��","ʩ��","Τ����","��չ��","���ݫh","��ά��","������","����֥","����","��ɽ��","�Ͽ���","������","�����","������","������","������","��ݼ��","������","������","������","�Ų���","Ԭ����","�϶���","ŷ����","֣��Ӱ","����","����ܽ","������","��ƻ֥","������","֣ӭ��","��ӿ��","�����","ʩ����","��ΰ��","��hѩ","̷��Ӣ","��ˬ��","�����","������","������","�����","����͢","��ţ��","��ῷ�","����ӳ","������","�Ű���","ŷ��","����ݿ","¬����","�ư���","��ĵ��","������","ʯ����","������","������","�����","����׳","Ϳ����","������","���ͬ","���","Σ����","½����","�뻱��","�˹���","�����","�ݶ�׿","�⾴��","��ټ˷","�η���","����","��ȼ��","᯻���","��짻�","��̩ά","�����","����ǧ","ŷ����","�߾���","������","�˰���","������","������","�ⲵ��","����ï","ŷ����","������","���ڸ�","��ة��","��ԭԸ","��Ľ��","ղ����","������","��ҵ��","������","���ܼ�","������","ʯ����","������","������","���","�̲���","�����","�̽���","��ʾ��","�Ž�Ʈ","�¾���","��˫��","Τ�Ծ�","��ΰ","������","ղ����","������","������","������","��Ե","��Ƶ١","������","�ܵ���","������","ʢ����","�����","�����","����","Ԫ����","��ţ��","�Ÿ���","��Ų�","������","���ٿ�","������","ׯ��","������","�˴���","���ﺭ","������","�ܴ�ݷ","������","����","������","��ʥ��","���ʕN","���","̷ҫ��","����ʥ","���߸�","ʢ��ɭ","��ȵ","��޴�","������","��׿��","������","ĪѶ��","�ο���","�����","Ҷĵ��","�Ҹ���","������","�ķ�","��˳��","ϯ��","�필�","���۳�","�ջ���","÷���","���ھ�","������","����Ⱦ","������","����","�޶���","½���","��ů��","������","��¶��","�����","������","������","��ʤ��","�̽�","�����","׿����","��·��","��Ӣ��","ʷ����","Ѧ����","���ֱ�","�Ƽ���","�°���","�����","�ű���","�Ÿ���","��ѩ��","������","����ѩ","��ӽӧ","������","������","�̵¿�","ŷ����","�ŵ���","����ԥ","����ݼ","������","������","����","��","��","�����","���Ӳ�","������","������","�ַ���","����ŵ","���ۺ�","������","�³���","�����","������","������","κ����","������","�⻶��","�ž���","���ݼ","��ҵ��","½����","���ܺ�","���Ի�","��Ƶ��","����Ӣ","֣��ö","��ܲʫ","��öӢ","ʩ��Ф","��ƱȺ","������","�찶��","��׿��","����ѩ","���ھ�","��Ⱥݺ","�����","Ǯ��ͬ","������","����ϼ","�̷���","���","������","Ǯ����","���Ӣ","�ſ���","���ڷ�","�����","��ˮ��","л��ŵ","����ө","Ѧ����","�̵ѻ�","�ž���","��ɯ","ŷ����","����","��Ը��","��Ӿ��","����һ","����ܿ","����ԥ","�����","ŷ���","����ܽ","������","����","������","����","��ӧ��","�¶���","ʷ����","��֮��","������","�܊�","��ٴ�","������","Ԭ��ޱ","����","�ܺ���","ʢ�ձ�","��ٻŵ","�׵���","��Ӱ��","��ޱ��","֣ѩݼ","���ᾧ","½����","������","������","������","�߾���","�ӱ̷�","���֥","��֮�","������","������","������","������","������","����ݼ","������","��ӭ��","��ϲ","³����","������","������","����","������","¬ǫ��","��ةӰ","Ǯ����","������","����ͬ","������","³����","��Ӫ��","����ɺ","�����","����ޱ","������","Σ����","�����","����õ","�¿�","����Ӱ","ͯ����","������","������","���ο�","������","����ö","����","��˳��","������","��̩","��ѷ��","������","��Ӿ��","�׾�˳","��ʾ��","ʩ���","�����","����Ⱦ","�����","������","�ܳ���","����","�ź���","�ʱ�","ׯ����","�Դ��","�ܲս�","�����","᯺���","�܃y��","�Ͼ�","�¾���","������","������","ʯ��ʥ","����","֣����","����","����Ⱦ","���ϸ�","�ֿ�","���ͱ�","������","������","�λ���","̷�ս�","������","�̼���","���ΰ","л�Ĺ�","̷�͕F","Ǯ��ѧ","������","ŷ���","������","���г�","�Ȿ","��ⰶ","������","���׵�","������","����֪","ͯ�˷�","������","�β���","�ո���","����˼","����ϸ","�����","������","½����","������","������","������","����Ȼ","�����","Ǯ����","ʯ��","�����","�����","ϯ�ϰ�","��˴Т","Ѧѩ��","�ϸ���","��Τ��","��ţ��","�����","����","������","³����","������","���޹�","������","ʢ����","������","��Ʒͮ","����ׯ","��Т��","ʷ����","������","���","��Ÿ��","������","Ѧ�ַ�","¬����","������","��ï��","�״���","�ŗ���","������","��ƽ��","�վ���","�հ人","������","л�к�","Ϳ����","������","���з�","�𹢻�","�ų�ӹ","�º�ɭ","��֦��","������","���Ź�","Ī����","���޵�","������","����ϯ","������","������","����ţ","������","������","·�̗�","������","᯸���","�ܱ���","�ձ���","��̫��","ͯ��˴","�����","�ι�͢","������","ŷ��ʩ","��Ư��","��骹�","����Ȼ","���ٱ�","������","������","���˷","�ܹ���","��ε��","����Խ","��־��","�޹̺�","���Ҿ�","����","������","���濬","����","�ﰽ��","Ҷ����","��ȵ��","������","���İ�","����ʷ","�����","���","������","��ݿ��","�����","������","���ι�","���ҹ�","��ӡ��","������","����","������","��˱�","�ű�","������","����","��","��","ղ���","�ĵ���","����˼","����Ʈ","κ����","������","������","����","�豦��","������","��³","·����","κ����","�̺���","ʯ��·","��չ��","Ѧɭ��","������","���ׯ","���Ѿ�","�����","������","������","����ҫ","��褼�","�»�ѩ","���˴","��Զ","������","������","�װ��","������","ǮƯ��","ʩ��Ǯ","������","����","��֦��","��ͥ��","��¥��","�̹���","������","��Ѷ��","����","������","������","�ź���","����Ʈ","������","������","̷˳��","����־","����Ȫ","���ϸ","������","���ٳ�","����ң","Ϳ��","��Ρ��","�θ���","����ϸ","����","��","��","������","������","Ԫ��Ϧ","����","��з�","������","ŷ���","����","�ʸ���","����ǿ","������","�ֿ���","������","������","��ǣ��","�Ѻ���","�̾�³","��ǧĵ","��Ƶ��","����ϯ","�׹�Խ","�ܳ�","����ɽ","������","��Ư��","������","������","�����","�ֽ��","���","����ǰ","��׻��","������","���ļ�","��ͮ��","������","����˼","������","�����","����ݿ","�ƿ�ά","������","���н�","�����","κ���","��¥��","ʱ�Ǻ�","��ˬ��","Ѧ֮��","������","������","��Ÿ��","ĪԴ��","�ӽ��","����˧","����Ȼ","���ݺ�","���ݿ�","����֦","������","���γ�","������","�̷���","����һ","������","������","������","κ��ܽ","�˰���","�ﵥܽ","ΤŪӢ","���ձ�","̷��ӭ","��ܰ��","���谮","����֪","�µ���","��ݼ��","��³��","�ѿ���","���","����Ů","������","�߾�ӱ","������","������","������","��˪��","�����","�׵���","�߱���","������","���״�","��Ѹ��","Ѧ���","���ĺ�","ŷ��","��ӽ��","���Ӱ","ͯͤ��","�α���","Ϳ����","������","������","��¥�","����ݺ","�¸���","κټ��","�����","������","�Ű���","л����","�����","������","������","��޲֥","�����","���Ǖ�","Ԭ����","���ط�","���ټ�","������","�ｭ��","������","������","��ĭ��","ʷ�ϴ�","�ʾ���","������","������","���","Ī����","÷����","Ԭ����","��ޱ��","᯶���","������","̷���","֣��ٻ","����Ӣ","ʱ֡��","������","�µ���","��ͯ��","������","֣Ÿ��","����Բ","�°���","������","�����","������","������","������","�����","��ȼ��","������","����ܿ","������","����ӱ","�ĵ�ӭ","������","�κ���","������","�ܸ���","�����","��˼��","����ӳ","������","������","���ƾ�","�ɰ���","��Է��","���꾲","��Ө��","������","�Ⱘ��","��԰ʫ","������","�̽���","ŷ����","��ȵ΢","���u��","ŷ��","ŷ","ѩ","������","���ؿ�","ղ���","����ʫ","������","�ɷ���","�Ű���","������","����ܰ","������","������","�η۽�","�׻���","������","������","ϯ����","л��õ","��ܰ��","��Բ��","�̼���","���л�","���㾲","��֦��","������","�����","�ž���","�Ը�ԥ","�����","����ʥ","������","������","������","������","�ѳѵ�","����ط","�����","�ֶ���","������","����ͼ","������","���ȸ�","�ι�׻","������","������","��־��","���Ƕ�","ʢ����","���н�","�����","��ϯƽ","����ׯ","�����","��ڽ�","����ȵ","�Ž���","�ž���","������","���","������","Ԫ»��","������","��Ľ��","�Ż���","�ѷ���","������","��ϣ��","����˼","�����","ʱ����","ŷҫ","��˴��","Ǯ����","�����","����","������","������","������","������","�����","��ϣ��","��Ԩ��","�ܱ���","������","����ɽ","�ܲݲ�","Σ��ԣ","���ï","������","������","������","���ź�","½��չ","���ĸ�","���ƿ�","��Ե��","Τ��Ÿ","������","Ԫ���t","�⹡��","���ͷ�","�ű���","����ũ","�����","�εȷ�","��Ʒ��","��ӳ��","�±���","��֮��","������","�ﱾ��","κ�꺺","ŷ�ٺ�","������","������","�Ҿ�","��᰺�","�����","��ݸ�","�±�˷","�Խ�ˮ","����","��ة��","�ױ��","����","���׾�","Ҷ����","������","�׵���","¬ӡ��","����Ը","���F��","ׯ����","�ѹ���","������","�Ź�ϯ","������","���İ�","������","ʩ����","�����","����ɽ","������","������","������","ʩ���","��ǫ�","��ݿ��","ͯ�þ�","�¼���","�ų���","�Ű���","������","������","�ϱ���","�ջ���","�ľ���","�־�Ȩ","��ң��","������","�԰���","��ǿ��","���ɱ�","������","ŷ��׿","Ǯ����","�����","����ټ","�ر���","����ʱ","������","������","�Ȳ���","����ԣ","��Ϧ��","�̲���","��ҫ��","�Ժ��","�ΰ���","�ﾰˬ","��ԣ��","�ΐ���","����","�����","��ӹ��","�ܶ��","��Ȼ��","÷��ʥ","������","������","��ʼ��","������","������","�޺ɸ�","��Цһ","������","������","������","������","Ѧ���","������","�޼���","������","����","���޷�","�����","��Ưţ","��ѡ��","��͢��","�߷���","�����","��ǧ��","�±���","������","������","�����","Ҷ��","������","��·","�Ļ���","�ϱ�֪","����ʱ","����","ղ��ׯ","������","Ī��","������","���⾢","�����","̷�ڸ�","�����","��μ��","�Ľ���","����","������","������","�޷�ά","ʯ�ܴ�","����Ʈ","������","����ά","����ͤ","��Ƶʱ","�����","������","����ͥ","������","��׻��","�׷ɽ�","������","������","Τ����","������","��¼��","�����","������","������","��ʼ��","ʢͮ��","�����","����׳","�׳���","����١","�����","ʯ����","ŷ����","ʱ���","·Ȩ��","ͯ����","ͯ�泿","������","�չ���","���編","������","������","������","����ѩ","�ܶ���","��ľ��","������","������","�Ұ���","������","Σ��ʷ","���Ŵ�","������","�ϰ�׳","��ˬ��","��ϲ��","������","������","��¼��","֣Ѷϫ","������","·���","Ԫ�ҳ�","ׯ����","��ũ˼","������","�ײ���","Ǯ��","����","�º�Ƚ","��׳��","�º���","������","�ܸ���","ղ����","��Ե","ʯ����","��譹�","ŷ����","������","ŷ��Ƚ","���޺�","���Ӻ�","������","��Ц��","������","������","�׳���","�붹��","�����","Ҷ����","��ͤ","������","������","��ӳ��","������","������","��÷��","֣���","�ΰ���","���","����ө","����","�ַ���","���","��","��","��ɺ��","��ѩ","��޲��","�Ž���","��ٻ","����ܿ","Ϳ��֪","���ݼ","������","����ع","����ܽ","ʩ����","��漾�","������","������","��Ե��","������","�澹�","�����","������","��¼��","������","᯷���","��˧��","������","��֥��","������","�״���","���信","�����","½��ӯ","���մ�","�ž�ö","����ӽ","����ݼ","���ɺ�","л����","��νŮ","������","���","����ܿ","����Ⱥ","�����","��ݼܿ","��ͥ��","����","�Ϸ�١","��ܿ��","ͯ��ԥ","¬����","�Ŀ�","���䰨","ׯ��ݼ","������","�����","������","����","��Ч��","��Ӧ԰","����¶","������","��ө��","�ⲯ��","������","������","��溴�","ʢ��ޱ","Ī˪��","�����","·԰ܿ","������","����","��ž�","������","�ϸ���","����Ȼ","����ͮ","���ǻ�","ϯ˳��","��ع��","�°���","��֮��","Σ����","�Ż���","���η�","Ԫ����","����Ө","����ԥ","������","������","��Ӿ֥","Σ쳷�","Ҷ����","�Ѹ���","��ö��","·ö��","�߻���","ʯ����","����ͮ","������","��޲˼","������","³ɯ�u","�Ļ�֥","��ľ��","������","������","·ѧҶ","�����","ղ����","�����","��Բө","��ӯ","�׻���","ŷ����","�����","������","������","�����","��С��","�ѽ��","������","������","�Ѱ���","������","����","�ܺ�","�����","�����","��N��","��˺�","��ӣ","���ӻ�","������","����֥","л˪��","�·�ݼ","ղ����","��õ��","������","л��Ӣ","�º���","�ּ���","����ܿ","������","������","���ԥ","᯽��","�ź�ͯ","᯴���","Σ��ü","��Ρ��","�����","�׼���","����ϼ","κ����","����","������","������","�Ŷ�ϼ","�ξ���","����ˬ","��ɾ�","����ݼ","̷����","����Ⱥ","������","����","Ǯӯө","�ɲ�ö","�׻ܾ�","Ԫȫ��","���ܽ","����","̷����","���Ļ�","����ĵ","��֮��","��ʼ��","������","�����","������","������","ʢ��ܿ","�����","ŷ�绷","�ĳ���","Ϳ�Ǵ�","Τ����","������","������","�����","ׯ��ӽ","������","֣��ѩ","·��","�°���","ŷ����","����","������","ʩ����","������","���","Ǯϫ��","��Ӧ��","��֡��","�����","л����","��Զ��","��֥��","����˪","Ԭ���","����ݼ","����","����","ŷ����","����ٻ","��ΰ��","���н�","��Ч��","�ѻ�","Σ����","������","����","�ܺ���","��ܰԥ","������","ŷ��Ӣ","����","�����","�ܰ���","������","�Ļ��","��˧ޱ","�¹���","������","����","Ǯ�Զ�","�̽㻷","����","�Ա�ϲ","�ϻ���","����ӱ","ʷ����","����ޱ","����ů","������","��ĺ��","�ް���","׿��","������","ŷ����","������","������","���澧","������","��ȺӢ","�½���","���","ʱ����","����֡","�����","������","������","������","��Ư��","������","����","Ҷ����","���ü�","����ޱ","�¼���","����U","��Ľ��","������","����","�ܶκ�","������","���ٻ","������","������","������","᯳���","��ͼܿ","��侸","������","������","������","����ܽ","��ϯط","�����","������","�����","���γ�","������","ͯ��Ÿ","�����","������","������","�»���","��˴��","�Ѳ���","¬��ׯ","����Ʒ","���ɴ�","������","ׯ����","Ǯ����","��ʱ","�谽��","���ǹ�","��ͨ�","������","����ɽ","������","�κ���","������","·����","�μ���","���庽","�˱���","½����","��ˬ�y","½�ֹ�","����","�Ų���","����·","·���","֣��Ծ","������","��չ��","·����","Ԫ��»","�̴���","�±���","����","�����","������","������","�Ʊ�","�����","�µ�˳","Τ��ϲ","��ϣ��","������","ŷ����","����Ʒ","�´��","������","³�˟�","��Τ��","������","��¼ˮ","�����","�׶���","����Ÿ","���׽�","�����","½ط��","��ϰ˧","������","�ع��","���յ�","��Ӱũ","��ׯ","������","����׼","������","Ҷ����","κ����","�ܿ���","�����","��Ӣ��","������","�ճ���","������","��Ѹ��","������","½����","��Ρ��","�ߺ��","������","ŷ��ε","���Ҳ�","Ī����","������","������","����","��ȫ��","¬����","���ݷ�","������","Ǯ�ǻ�","�ܱ��","������","��ȵ��","ղ����","����Ƚ","�س���","��𩸢","��ӿ","������","������","�𥉉��","���䰲","�ķ���","�ż���","������","����ʤ","�¶���","����","��ϸ��","ŷ����","³����","����","����׼","������","���հ�","��ѧ��","����","�Ƴ���","������","������","������","������","ͯ��Ÿ","�³���","��Ե��","����ͤ","�ش�","������","�̱�ѷ","�ܼ���","����Ȩ","ղ�ֶ�","�¾���","���ɼ�","���ޱ�","�Ӱ���","�����","�����","������","�֦","������","ʢ����","��ո�","��˳��","��ΰ͢","�ֲ���","���ǹ�","��ӿ��","������","Ǯ��","�����","��ҵ��","�׵���","�γ���","��Ѷ��","�����","��ҵ��","Ҷ����","���ո�","���F��","����","������","��˴��","������","��׼��","������","������","�ū���","������","��ʼ��","�ŷ���","������","��ͤ","�˸�Ӿ","Ǯ��ŵ","����ϲ","����","ͯ����","�¥","������","�ֿ�ͥ","��ŵ̫","ʱ����","�����","��ة��","��ά","Ϳ�º�","�ܻ㲨","������","������","�����","׿¥̩","�����","����","������","�����","������","�����t","��ұ��","�Է�","������","����ݿ","�̰���","ʢ��ݿ","������","�콭��","������","������","��Ը��","���Ӱ�","ʢ�޶�","������","�Ѱ���","������","�źӴ�","���","������","ʩ�ӿ�","����ͯ","������","����ε","��裹�","������","��ˬ��","׿����","����ѩ","Ҷ����","�ıѻ�","������","̷��Խ","������","�����","��ľ","������","��ךּ�","�ι۱�","�����","ʯ����","�ܰ���","������","����Ǯ","�����","���ٹ�","�����","������","�˻���","ŷ����","�׸���","�ν���","���ٺ�","�±���","�����","������","����","������","ŷ�˾�","������","����˼","������","������","�ֱ�ɽ","��ң��","᯽�","��ţӢ","����","�¸���","������","Ǯ����","���ʸ�","�Ľ츪","������","�����","������","���н�","������","����ï","������","�α㲮","ʩ��ξ","Σ���","���ٻ�","�����","��Դ��","Ԫ����","�³�","�����","�ذ׷�","�ظ���","�����","��ʯ��","������","�κ�","ղ�Ƶ�","��鷢","ŷ����","���｡","�Ϻ��","�ξ���","�벮��","��ν��","��ε��","Σ����","�Ĳ���","������","�ܸ�־","���ӽ","��ͼ��","���Ⱥ�","Ԫ�ֱ�","��ѧ��","������","¬����","������","������","������","���ڿ�","��ϧϫ","������","Σ����","������","�����","������","Ǯ˼��","���Ķ�","�����","���ͼ","������","������","�����","����Ʒ","�α���","ׯ����","�����","������","���Ʈ","������","�����","������","�ܷ�ʱ","������","�Ÿ���","��ɺ�","������","�ڼ̹�","����","������","������","������","÷��","������","��ʼ֪","��³־","�����","�ְ׾�","��߸�","��ˮ��","�Ż���","ԪԪ��","ׯ�Ƹ�","�԰���","������","�����","�����","�׽���","������","������","������","ʯ��Ʒ","���Ƿ�","�ߕ���","��¶��","�菪��","������","�����","��ׯ�","���㻴","�θ���","����","�����","������","��֪��","����","������","������","������","ʷ����","���ǿ","���ڷ�","�߶���","ϯ׿��","������","������","ѦӢ��","������","������","������","�����","���භ","Ҷ짻�","��Ч��","���됪","�¾���","�׽���","����","����ά","�ű���","������","������","�𵳸�","Ī����","��Ң��","�����","�����]","���絳","����ط","������","������","����ʿ","��֦��","·����","����˷","������","½Ӣʷ","�����","�̳�ʷ","������","���·�","�뻢�","Ԭ����","��»��","�����","������","��֪","������","������","�ر���","������","������","��ũ��","��㴫","��ӿ��","����","�Ƹ�̫","���","¬ԭ��","���׺�","����","��ӡ��","֣��","�����","���齭","���Ŭ","�����","������","��쿷�","�·�ϣ","�����","������","���Ͻ�","���ӻ�","������","�ſ���","�ĸ���","���ܺ�","��һӱ","������","�����","���˴�","ŷ����","������","����","�����","Ϳ����","������","����","�ƾ���","������","���Ĳ�","Ǯ��","��짼�","����ά","����ҹ","������","���ʤ","������","½����","������","�����","��Ⱥɽ","�ֲ�»","ʩ�ɐ�","��׳��","ׯѩ�","���Ҹ�","�̿�ͥ","������","��˹��","�ư�Զ","�˸���","�϶�","������","������","�ƴ���","��ӹ��","�����","������","������","�����","����","�·���","���ľ","��Ȱ��","�ɽ���","��ӿ��","�݁���","ʩ��","����ˮ","ׯԪ","Ѧ����","������","�ױߺ�","�ի���","�Ϸ�","����ϲ","�����","������","�����","�˻���","�ķ���","�ұϸ�","���꺮","������","������","������","���ȳ�","�ֽ���","������","������","ϯ��ʩ","������","�˴���","������","���Ų�","������","����ׯ","����ţ","�ʰ���","����","ŷ����","��Ը��","�ƽ���","�ּ���","�����","ʢԽ��","��Ӧ��","л����","׿�Ⱥ�","��ȹ�","������","�����","����ϣ","������","�̹�","���ζ�","���ͤ","�����","������","�ط���","�ܱؾ�","������","������","������","�����","������","����ӹ","Īѭ��","��ɽ��","����Ⱥ","ϯ��ӹ","����չ","������","���ĺ�","���ָ�","�¼�","������","��̫��","������","�Ű���","�ܻ���","�ΰ���","Ī��ѩ","������","������","���ֻ�","�ɶ���","�Ŷ�","�����","��짶�","������","�ѷ���","����ʢ","������","¬����","�´�¥","������","��Ʈ��","���鿡","���ҫ","������","�̶��","������","���ʸ�","�ϴ���","�谪��","����Դ","��ة�","��ѧ","����ǫ","κ��","������","������","��һ��","�ϵ��","�����","������","�����","ʷ�ҳ�","����","�ػ�ï","�߳���","�̱�ط","�ܱ�ط","¬�","���ܰ�","������","��屸","��Ľ��","��ţǮ","�����","�ź���","�Ͼ�³","ׯ����","����","������","�ɹ���","���ϼ�","������","������","�ű���","κ��Τ","����ˮ","�����","������","��ѷӽ","����","�¾���","�ű���","�̰���","��衸�","������","������","Ԫ����","��ԭԪ","������","½����","�����","�׾���","�ܽ���","��ѡǫ","ʱ�ҳ�","������","¬����","������","������","����","��ةξ","�����","������","����ϸ","������","��ع�","��Ң��","�η��","��³��","������","�ɻ���","������","֣���","����","׿��ӳ","�̻���","½��","��Ÿ��","���Ѷ","������","Ԭ��͢","�･Ρ","ŷ����","����ݼ","�����","���ЕF","��ѩ��","�����","����Ρ","�ظǻ�","���Ɉ�","�����","�³���","������","��˧��","������","�ϱ���","Ԫ����","�ش�Ȫ","������","����","�����","Ԫ����","��ҹ��","�ֿ���","����ǿ","��Ե��","������","���Ǻ�","���϶�","������","��ϯ��","����","Τ�۱�","������","̷�ϼ�","�϶θ�","���ʹ�","��·ˬ","Ԭǣ��","��ĵԴ","������","��ϧ��","������","�����","��ͨ��","�Գ���","Ǯ����","������","������","��Ľݷ","������","��Ԫ��","������","����ɭ","ŷ����","�ŷ���","Ҷ����","��貺�","�Ҵ��","Ϳ����","������","�ɹ��","³�Ǿ�","������","��ǣ��","��ũȾ","������","��Դ��","���","��","��","�ݲ���","������","������","�Ժ���","��Ȼ��","���ֲ�","������","������","ŷ����","�Ľ���","������","�̴���","����˼","��ѩ��","�Ÿ���","������","������","Τ����","��Ǯ��","�ظ���","�ݾ���","ϯ����","������","��͹�","���ǲ�","������","��ά��","κ�Ӳ�","�̳�ԭ","��̫��","���Դ�","����","������","������","�ҷ�","¬���","��Ρ��","������","����ҹ","�����","��Ӫ��","Ԫ�ӱ�","���³","������","�屸��","֣Զ��","����","¬����","�Ųٹ�","���ƹ�","ʩ��","�̹���","������","�ݹ���","��߻�","ŷҡ��","������","������","��»��","������","ʷ����","����˷","�����","�����","�𻻱�","����","Σ̫��","������","��˳��","�ܱ���","�����","������","�����","¬����","�����","��᰻�","������","Ѧӱ��","������","ϯҫ��","��ĵ��","��ε��","�Ե���","�̹�","�Ļո�","������","����Σ","�ﵭŬ","������","������","�°���","��Τ��","�ⱶ˹","�����","�Ґ���","������","������","�¶�ӳ","����ͯ","ʩ����","�׳ذ�","���½�","������","����Τ","Τ����","��Ʒ��","Ԫ��̩","�����","����˳","ŷ����","�¸���","������","��·��","������","��ʾ��","������","����","������","�ϸ��","��̫��","������","����Զ","������","ǮƯ��","��֥","Ī��ѩ","�����","׿����","÷����","���","��ǧʫ","��׿��","��ة�","������","����ˬ","�����","������","����ѩ","Τ����","��¡��","���Ρ","����","�̿��","��ݼͮ","����ӳ","�Ϻ���","��־��","��Ϧ�","��ܺ�","½����","��˼��","������","�Ÿ���","�ſ�","ᯫh��","����","��ҹ��","÷����","��ޱ��","�����","����","��÷��","������","�ܷ淽","��Ŭŵ","�ֳ�ܲ","ʢ���","��ü��","������","�ַ���","ŷ֮��","�Ͼ���","������","л��","֣����","��ޱ��","������","����ɺ","����֥","���ȴ�","����","����ٳ","�˳���","���ܲ","ϯ襾�","����","������","������","�����","½��Ө","��ɺ��","������","��־��","��ӢԶ","������","½����","��޲��","�����","��Ʊ��","��Ҷ֡","���κ�","������","������","�Ʒ���","��콾�","����","��","˫","����ŵ","����","��","ͮ","�Ŵ���","������","�����","�����","���","����","����ά","����ӳ","������","½����","����ͯ","����ܽ","������","��ҹʫ","������","ͯ�ƺ�","�ﵤӳ","������","�����","ׯ�Ĵ�","÷����","����","������","��溾�","�����","ŷ����","·�ٱ�","��˴��","�����","���˫","�ղ���","�¿�","ʱ�ݺ�","������","κ����","������","������","���Ӿ","������","����֥","����ů","������","����ͯ","�ܳ���","�����","½��ͬ","�׺��","�����","������","������","��ϲ��","������","����Ӱ","¬��ٻ","����֥","׿��Ը","�����","ŷ���","ׯ����","ͿϧӢ","�ؽ���","�����","�����","�ܼ�˫","������","�ܗ���","������","³�ؾ�","������","����ѩ","�����","��ɯ��","���U��","����Ӣ","֣��Ⱥ","�ܺ�ӯ","�ݽ���","Ϳ����","�����","��ܲӰ","Σ����","ׯ����","ϯѤ��","����ܿ","������","��Ư��","������","���׾�","��ݼ��","ղ����","��˧ܿ","�ݳ���","�����","������","�ŷ���","�����","�԰���","����ٻ","ղ����","������","����","����ŵ","����","������","������","��ѧ��","л��","����˼","���Ӣ","�Ӿ���","����ŵ","¬Ӣ��","κ����","����","������","��ө��","�����","ŷ����","���ٻ","����","�׵���","�±���","����ѩ","�����","������","÷��ͬ","�����","������","��Ȼ�h","�����","��͢��","¬Է��","���»�","��ѭ˼","�ﵳޱ","�����","������","������","��ǣ��","��浻�","������","���ܴ�","������","����ԥ","�����","Ԫ����","����ӳ","�����","����ů","ᯊ���","������","�Ϻɴ�","�����","�ܵ��","���ԥ","�̶���","᯹���","����ۼ","������","��ͨٳ","����԰","��ʷ��","����","��ӿͮ","Ϳ�݊�","���Ӣ","½��ݼ","�ܽ���","������","��Ӣ��","������","ʯ�h��","Ҷ��Ң","��Τ��","������","��ƮȺ","�����","���Ӱ","������","÷��Ӣ","½���","����","Ԫ����","������","������","������","����֦","������","÷����","������","�ﾲܽ","����ޱ","������","����ε","������","Σ���","�ܶ��","������","ǮӦ�","���㻶","Ϳ����","������","����ܿ","������","����Ӣ","������","�ܶ�ܽ","������","��ط��","ʩ��ө"};
	const int nCount = sizeof(s_names);
	srand((unsigned int)_time64(NULL));
	int nIndex = rand();
	CString str;
	str.Format(_T("%d"), nCount);
	return CString(CA2T(s_names[ nIndex % (nCount/sizeof(const char*))]));
}

CString IBAGlobal::GetCurrentDateTime()
{
	SYSTEMTIME sm;
	GetSystemTime(&sm);

	CString str;
	str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), sm.wYear, sm.wMonth, sm.wDay, sm.wHour, sm.wMinute, sm.wSecond);

	return str;
}

CString IBAGlobal::StringValueForKey( const char* key, GxxDictionaryPtr dicPtr )
{
	if (dicPtr) {
		GxxValue* pValue= dicPtr->valueForKey(key);
		if (pValue) {
			CString str(G2XUtility::CG2XCommonTool::Utf8_to_WideChar(std::string(pValue->stringValue())).c_str());
			return str;
		}
	}
	return _T("");
}
