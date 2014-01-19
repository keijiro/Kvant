using UnityEngine;
using System.Collections;

public class KvantNormalDeformer : MonoBehaviour
{
    public bool smooth = false;
    public float noise = 0.5f;
    public float scale = 0.5f;
    public float speed = 0.5f;
    Mesh mesh;
    Vector3[] sourceVertices;
    Vector3[] sourceNormals;
    int[] sourceIndices;
    Vector3[] vertices;
    Vector3[] outVertices;

    void Awake ()
    {
        var meshFilter = GetComponent<MeshFilter> ();
        var source = meshFilter.sharedMesh;

        sourceVertices = source.vertices;
        sourceNormals = source.normals;
        sourceIndices = source.GetIndices (0);

        mesh = new Mesh ();
        mesh.MarkDynamic ();

        if (smooth)
        {
            vertices = new Vector3[sourceVertices.Length];
            mesh.vertices = vertices;
            mesh.SetIndices (sourceIndices, MeshTopology.Triangles, 0);
        }
        else
        {
            vertices = new Vector3[sourceVertices.Length];
            outVertices = new Vector3[sourceIndices.Length];
            var indices = new int[sourceIndices.Length];
            for (var i = 0; i < indices.Length; i++)
                indices [i] = i;
            mesh.vertices = outVertices;
            mesh.SetIndices (indices, MeshTopology.Triangles, 0);
        }

        meshFilter.sharedMesh = mesh;
    }

    void Update ()
    {
        var offs = Vector3.forward * Time.time * speed;

        for (var i = 0; i < vertices.Length; i++)
        {
            var sv = sourceVertices [i];
            var crd = (sv + offs) * scale;
            var disp = 0.0f;
            var w = 1.0f;
            for (var oct = 0; oct < 4; oct++)
            {
                disp += Kvant.Noise (crd) * w;
                w *= 2.0f;
                crd *= 2.0f;
            }
            vertices [i] = sv + sourceNormals [i] * disp * noise;
        }

        if (smooth)
        {
            mesh.vertices = vertices;
        }
        else
        {
            for (var i = 0; i < sourceIndices.Length; i++)
            {
                outVertices[i] = vertices[sourceIndices[i]];
            }
            
            mesh.vertices = outVertices;
        }

        mesh.RecalculateNormals ();
    }
}
