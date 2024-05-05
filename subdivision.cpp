#include "subdivision.h";

// Hash function for glm::vec3, taken from internet
struct Vec3Hash {
	size_t operator()(const glm::vec3& v) const {
		// Multiplicative constants to spread out the bits
		const int hashX = 73856093;
		const int hashY = 19349663;
		const int hashZ = 83492791;

		// Convert each component to an integer hash
		size_t hash = std::hash<float>{}(std::round(v.x * hashX)) ^
			std::hash<float>{}(std::round(v.y * hashY)) ^
			std::hash<float>{}(std::round(v.z * hashZ));

		return hash;
	}
};

struct SmartVertList {
	std::vector<glm::vec3> verts;
	std::unordered_map<glm::vec3, int, Vec3Hash> vert_indexes;

	int push(glm::vec3 vert) {
		if (vert_indexes.find(vert) == vert_indexes.end()) {
			verts.push_back(vert);
			vert_indexes[vert] = verts.size() - 1;
		}
		return vert_indexes[vert];
	}
};

// Custom hash function for pair<int, int>, taken from internet
struct PairHash {
	size_t operator()(const std::pair<int, int>& p) const {
		size_t h1 = std::hash<int>{}(p.first);
		size_t h2 = std::hash<int>{}(p.second);
		// Combine the hash values of the two integers
		return h1 ^ (h2 << 1);
	}
};

std::pair<int, int> me(int i1, int i2) {
	if (i1 > i2) {
		std::swap(i1, i2);
	}
	return { i1, i2 };
}

glm::vec3 qm(const Mesh& mesh, const Quad& quad) {
	const auto& p1 = mesh.verts[quad.vi1];
	const auto& p2 = mesh.verts[quad.vi2];
	const auto& p3 = mesh.verts[quad.vi3];
	const auto& p4 = mesh.verts[quad.vi4];
	return (p1 + p2 + p3 + p4) / 4.0f;
}

glm::vec3 tm(const Mesh& mesh, const Triangle& tri) {
	const auto& p1 = mesh.verts[tri.vi1];
	const auto& p2 = mesh.verts[tri.vi2];
	const auto& p3 = mesh.verts[tri.vi3];
	return (p1 + p2 + p3) / 3.0f;
}

glm::vec3 em(const Mesh& mesh, const std::pair<int, int>& edge) {
	const auto& p1 = mesh.verts[edge.first];
	const auto& p2 = mesh.verts[edge.second];
	return (p1 + p2) / 2.0f;
}

glm::vec3 formula_q_quad(const Mesh& mesh, std::unordered_map<int, std::vector<Quad>>& nqp, int vi) {
	glm::vec3 total{};
	for (const auto& nq : nqp[vi]) {
		total += qm(mesh, nq);
	}
	return total / (float)nqp[vi].size();
}

glm::vec3 formula_q_tri(const Mesh& mesh, std::unordered_map<int, std::vector<Triangle>>& nqp, int vi) {
	glm::vec3 total{};
	for (const auto& nq : nqp[vi]) {
		total += tm(mesh, nq);
	}
	return total / (float)nqp[vi].size();
}

glm::vec3 formula_r(const Mesh& mesh, std::unordered_map<int, std::unordered_set<std::pair<int, int>, PairHash>>& nep, int vi) {
	glm::vec3 total{};
	for (const auto& ne : nep[vi]) {
		total += em(mesh, ne);
	}
	return total / (float)nep[vi].size();
}

glm::vec3 calc_vert_move_quad(const Mesh& mesh,
	std::unordered_map<int, std::vector<Quad>>& nqp,
	std::unordered_map<int, std::unordered_set<std::pair<int, int>, PairHash>>& nep,
	int vi) {
	float valence = (int)nep[vi].size();
	return (formula_q_quad(mesh, nqp, vi) / (float)valence) + ((2.0f * formula_r(mesh, nep, vi)) / valence) + (((valence - 3.0f) * mesh.verts[vi]) / valence);
}

glm::vec3 calc_vert_move_tri(const Mesh& mesh,
	std::unordered_map<int, std::vector<Triangle>>& nqp,
	std::unordered_map<int, std::unordered_set<std::pair<int, int>, PairHash>>& nep,
	int vi) {
	float valence = (int)nep[vi].size();
	return (formula_q_tri(mesh, nqp, vi) / (float)valence) + ((2.0f * formula_r(mesh, nep, vi)) / valence) + (((valence - 3.0f) * mesh.verts[vi]) / valence);
}

Mesh subdivide_tri(Mesh mesh) {
	Mesh result;
	std::unordered_map<std::pair<int, int>, std::vector<Triangle>, PairHash> nte;
	std::unordered_map<int, std::vector<Triangle>> ntp;
	std::unordered_map<int, std::unordered_set<std::pair<int, int>, PairHash>> nep;

	for (const auto& t : mesh.tris) {
		auto e1 = me(t.vi1, t.vi2);
		auto e2 = me(t.vi2, t.vi3);
		auto e3 = me(t.vi3, t.vi1);

		nte[e1].push_back(t);
		nte[e2].push_back(t);
		nte[e3].push_back(t);

		ntp[t.vi1].push_back(t);
		ntp[t.vi2].push_back(t);
		ntp[t.vi3].push_back(t);

		nep[t.vi1].insert(e1);
		nep[t.vi1].insert(e3);
		nep[t.vi2].insert(e1);
		nep[t.vi2].insert(e2);
		nep[t.vi3].insert(e2);
		nep[t.vi3].insert(e3);
	}

	SmartVertList sl;

	for (const auto& t : mesh.tris) {
		auto tri_mid = tm(mesh, t);

		auto e1 = me(t.vi1, t.vi2);
		auto e2 = me(t.vi2, t.vi3);
		auto e3 = me(t.vi3, t.vi1);
		auto ep_1 = (tm(mesh, nte[e1][0]) + tm(mesh, nte[e1][1]) + mesh.verts[e1.first] + mesh.verts[e1.second]) / 4.0f;
		auto ep_2 = (tm(mesh, nte[e2][0]) + tm(mesh, nte[e2][1]) + mesh.verts[e2.first] + mesh.verts[e2.second]) / 4.0f;
		auto ep_3 = (tm(mesh, nte[e3][0]) + tm(mesh, nte[e3][1]) + mesh.verts[e3.first] + mesh.verts[e3.second]) / 4.0f;

		auto v1 = calc_vert_move_tri(mesh, ntp, nep, t.vi1);
		auto v2 = calc_vert_move_tri(mesh, ntp, nep, t.vi2);
		auto v3 = calc_vert_move_tri(mesh, ntp, nep, t.vi3);

		// f1
		auto norm = qn(v1, ep_1, tri_mid, ep_3);
		result.quads.push_back(Quad(sl.push(v1), sl.push(ep_1), sl.push(tri_mid), sl.push(ep_3)));

		// f2
		norm = qn(ep_1, v2, ep_2, tri_mid);
		result.quads.push_back(Quad(sl.push(ep_1), sl.push(v2), sl.push(ep_2), sl.push(tri_mid)));

		// f3
		norm = qn(tri_mid, ep_2, v3, ep_3);
		result.quads.push_back(Quad(sl.push(tri_mid), sl.push(ep_2), sl.push(v3), sl.push(ep_3)));
	}

	result.verts = sl.verts;

	return result;
}

Mesh subdivide_quad(Mesh mesh) {
	Mesh result;
	std::unordered_map<std::pair<int, int>, std::vector<Quad>, PairHash> nqe;
	std::unordered_map<int, std::vector<Quad>> nqp;
	std::unordered_map<int, std::unordered_set<std::pair<int, int>, PairHash>> nep;

	for (const auto& q : mesh.quads) {
		auto e1 = me(q.vi1, q.vi2);
		auto e2 = me(q.vi2, q.vi3);
		auto e3 = me(q.vi3, q.vi4);
		auto e4 = me(q.vi4, q.vi1);

		nqe[e1].push_back(q);
		nqe[e2].push_back(q);
		nqe[e3].push_back(q);
		nqe[e4].push_back(q);

		nqp[q.vi1].push_back(q);
		nqp[q.vi2].push_back(q);
		nqp[q.vi3].push_back(q);
		nqp[q.vi4].push_back(q);

		nep[q.vi1].insert(e1);
		nep[q.vi1].insert(e4);
		nep[q.vi2].insert(e1);
		nep[q.vi2].insert(e2);
		nep[q.vi3].insert(e2);
		nep[q.vi3].insert(e3);
		nep[q.vi4].insert(e3);
		nep[q.vi4].insert(e4);
	}

	SmartVertList sl;

	for (const auto& q : mesh.quads) {
		auto quad_mid = qm(mesh, q);

		auto e1 = me(q.vi1, q.vi2);
		auto e2 = me(q.vi2, q.vi3);
		auto e3 = me(q.vi3, q.vi4);
		auto e4 = me(q.vi4, q.vi1);
		auto ep_1 = (qm(mesh, nqe[e1][0]) + qm(mesh, nqe[e1][1]) + mesh.verts[e1.first] + mesh.verts[e1.second]) / 4.0f;
		auto ep_2 = (qm(mesh, nqe[e2][0]) + qm(mesh, nqe[e2][1]) + mesh.verts[e2.first] + mesh.verts[e2.second]) / 4.0f;
		auto ep_3 = (qm(mesh, nqe[e3][0]) + qm(mesh, nqe[e3][1]) + mesh.verts[e3.first] + mesh.verts[e3.second]) / 4.0f;
		auto ep_4 = (qm(mesh, nqe[e4][0]) + qm(mesh, nqe[e4][1]) + mesh.verts[e4.first] + mesh.verts[e4.second]) / 4.0f;

		auto v1 = calc_vert_move_quad(mesh, nqp, nep, q.vi1);
		auto v2 = calc_vert_move_quad(mesh, nqp, nep, q.vi2);
		auto v3 = calc_vert_move_quad(mesh, nqp, nep, q.vi3);
		auto v4 = calc_vert_move_quad(mesh, nqp, nep, q.vi4);

		result.quads.push_back(Quad(sl.push(v1), sl.push(ep_1), sl.push(quad_mid), sl.push(ep_4)));
		result.quads.push_back(Quad(sl.push(ep_4), sl.push(quad_mid), sl.push(ep_3), sl.push(v4)));
		result.quads.push_back(Quad(sl.push(ep_1), sl.push(v2), sl.push(ep_2), sl.push(quad_mid)));
		result.quads.push_back(Quad(sl.push(quad_mid), sl.push(ep_2), sl.push(v3), sl.push(ep_3)));
	}

	result.verts = sl.verts;

	return result;
}

Mesh subdivide(Mesh mesh) {
	if (mesh.tris.size() > 0) {
		return subdivide_tri(mesh);
	}
	else {
		return subdivide_quad(mesh);
	}
}

Mesh subdivide_level(Mesh mesh, int level) {
	Mesh result = mesh;
	for (int i = 0; i < level; i++) {
		result = subdivide(result);
		//save_obj_debug(result, "debug" + std::to_string(i) + ".obj");
	}
	return result;
}
